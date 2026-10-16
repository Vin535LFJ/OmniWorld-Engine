#pragma once
// Metrics module — deep module (L1 sec 2; arch-review candidate #2 adopted):
// one feed entry hides N pluggable metric computations behind a tiny
// interface. Registration is the only extension point; the core never
// learns about a new metric.
#include "belief/ledger.hpp"
#include "world_state/world_state.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ow {

class Metric {
 public:
  virtual ~Metric() = default;
  virtual const char* name() const = 0;
  virtual void feed(const WorldState&, const BeliefLedger&, Tick) = 0;
  virtual double value() const = 0;
  // Multi-key emission (default: one key, name() -> value()). Metrics with a
  // decomposed view (e.g. per-producer ECE) override this; report() collects
  // through it so both aggregate and decomposed keys land in one map.
  virtual void collect(std::map<std::string, double>& out) const {
    out[name()] = value();
  }
};

class MetricsModule {
 public:
  MetricsModule();  // registers divergence / ece / identity_stability
  void register_metric(std::unique_ptr<Metric> m);
  void feed(const WorldState&, const BeliefLedger&, Tick);
  std::map<std::string, double> report() const;  // name -> value, sorted

 private:
  std::vector<std::unique_ptr<Metric>> metrics_;
};

}  // namespace ow
