#include "metrics/metrics.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <map>

namespace ow {
namespace {

constexpr float kCorrectTol = 0.1f;  // "positionally correct" bar (v1, fixed)

// Position error of a matched proposal against current truth (x/z plane).
// Returns false when the entry cannot carry a position claim.
bool matched_error(const BeliefEntry& e, const WorldState& ws, double& err) {
  if (e.verdict.kind != VerdictKind::Matched || !e.proposal.fields.transform)
    return false;
  const Entity* truth = ws.find(e.verdict.matched);
  if (truth == nullptr) return false;
  const double dx = static_cast<double>(e.proposal.fields.transform->position.x) -
                    static_cast<double>(truth->transform.position.x);
  const double dz = static_cast<double>(e.proposal.fields.transform->position.z) -
                    static_cast<double>(truth->transform.position.z);
  err = std::sqrt(dx * dx + dz * dz);
  return true;
}

class DivergenceMetric final : public Metric {
 public:
  const char* name() const override { return "divergence"; }
  void feed(const WorldState& ws, const BeliefLedger& l, Tick t) override {
    for (const BeliefEntry* e : l.at(t)) {
      double err = 0.0;
      if (matched_error(*e, ws, err)) {
        sum_ += err;
        n_ += 1.0;
      }
    }
  }
  double value() const override { return n_ > 0.0 ? sum_ / n_ : 0.0; }

 private:
  double sum_ = 0.0;
  double n_ = 0.0;
};

class EceMetric final : public Metric {
 public:
  const char* name() const override { return "ece"; }
  void feed(const WorldState& ws, const BeliefLedger& l, Tick t) override {
    for (const BeliefEntry* e : l.at(t)) {
      const float c = e->proposal.confidence;
      const int bin = std::min(9, static_cast<int>(std::floor(c * 10.0f)));
      Bin& b = bins_[e->proposal.producer][bin];  // producer-keyed rows
      b.n += 1.0;
      b.conf_sum += static_cast<double>(c);
      double err = 0.0;
      const bool correct = matched_error(*e, ws, err) &&
                           err <= static_cast<double>(kCorrectTol);
      if (correct) b.correct += 1.0;
    }
  }
  double value() const override {
    double total = 0.0;
    double ece = 0.0;
    for (const auto& [producer, bs] : bins_) {
      for (const Bin& b : bs) {
        total += b.n;
        ece += std::abs(b.correct - b.conf_sum);
      }
    }
    return total > 0.0 ? ece / total : 0.0;
  }
  // Aggregate key "ece" plus one decomposed key per producer ("ece:<name>").
  void collect(std::map<std::string, double>& out) const override {
    out["ece"] = value();
    for (const auto& [producer, bs] : bins_) {  // map order = deterministic
      double n = 0.0;
      double ece = 0.0;
      for (const Bin& b : bs) {
        n += b.n;
        ece += std::abs(b.correct - b.conf_sum);
      }
      if (n > 0.0) out["ece:" + producer] = ece / n;
    }
  }

 private:
  struct Bin {
    double n = 0.0;
    double conf_sum = 0.0;
    double correct = 0.0;
  };
  std::map<std::string, std::array<Bin, 10>> bins_;
};

class IdentityStabilityMetric final : public Metric {
 public:
  const char* name() const override { return "identity_stability"; }
  void feed(const WorldState&, const BeliefLedger& l, Tick t) override {
    std::vector<EntityId> cur;
    for (const BeliefEntry* e : l.at(t)) {
      if (e->verdict.kind == VerdictKind::Matched) {
        cur.push_back(e->verdict.matched);
      }
    }
    std::sort(cur.begin(), cur.end());
    if (have_prev_) {
      ++transitions_;
      if (cur == prev_) ++stable_;
    }
    prev_ = std::move(cur);
    have_prev_ = true;
  }
  double value() const override {
    // No adjacent pair observed yet: declare full stability (L2 sec 5).
    return transitions_ > 0.0 ? stable_ / transitions_ : 1.0;
  }

 private:
  std::vector<EntityId> prev_;
  bool have_prev_ = false;
  double transitions_ = 0.0;
  double stable_ = 0.0;
};

}  // namespace

MetricsModule::MetricsModule() {
  register_metric(std::make_unique<DivergenceMetric>());
  register_metric(std::make_unique<EceMetric>());
  register_metric(std::make_unique<IdentityStabilityMetric>());
}

void MetricsModule::register_metric(std::unique_ptr<Metric> m) {
  metrics_.push_back(std::move(m));
}

void MetricsModule::feed(const WorldState& ws, const BeliefLedger& l, Tick t) {
  for (const auto& m : metrics_) m->feed(ws, l, t);
}

std::map<std::string, double> MetricsModule::report() const {
  std::map<std::string, double> out;
  for (const auto& m : metrics_) m->collect(out);
  return out;
}

}  // namespace ow
