#include "grounding_loop/phase1.hpp"

#include "core/hash.hpp"
#include "models/rollout.hpp"
#include "models/world_model.hpp"
#include "truth_world/truth_world.hpp"
#include "world_state/world_state.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <map>
#include <utility>

namespace ow {
namespace {
using clock_type = std::chrono::steady_clock;

double secs_since(clock_type::time_point t0) {
  return std::chrono::duration<double>(clock_type::now() - t0).count();
}
double micros_since(clock_type::time_point t0) {
  return std::chrono::duration<double, std::micro>(clock_type::now() - t0)
      .count();
}

double percentile(std::vector<double> v, double p) {
  if (v.empty()) return 0.0;
  std::sort(v.begin(), v.end());
  const double idx = p * double(v.size() - 1);
  const std::size_t lo = static_cast<std::size_t>(idx);
  const std::size_t hi = std::min(lo + 1, v.size() - 1);
  const double frac = idx - double(lo);
  return v[lo] * (1.0 - frac) + v[hi] * frac;
}

std::uint64_t rollout_digest(const Rollout& r) {
  Hasher h;
  h.label("Rollout");
  h.add_u64(r.seed);
  h.add_u64(static_cast<std::uint64_t>(r.n_bodies));
  h.add_u64(r.ticks.size());
  for (const RolloutTick& t : r.ticks) {
    h.add_u64(t.tick);
    h.add_u64(t.entities.size());
    for (const RolloutEntity& e : t.entities) {
      h.add_u64(e.id);
      h.add_u32(static_cast<std::uint32_t>(e.class_index));
      h.add_f32(e.pos.x);
      h.add_f32(e.pos.y);
      h.add_f32(e.vel.x);
      h.add_f32(e.vel.y);
    }
  }
  return h.h;
}

}  // namespace

Phase1Result run_world_model_eval(const Phase1Config& cfg) {
  Phase1Result res;
  res.seed = cfg.seed;
  res.n_bodies = cfg.n_bodies;
  res.ticks = cfg.ticks;

  // 1) record the truth rollout (training data + replay oracle)
  const Rollout rollout = record_rollout(cfg.seed, cfg.n_bodies, cfg.ticks);
  res.rollout_digest = rollout_digest(rollout);

  // 2) instantiate all world models via the registry and fit them
  auto models = create_all_world_models();  // sorted by name
  std::map<std::string, ModelEval> evals;
  for (auto& m : models) {
    ModelEval e;
    e.model = m->name();
    const auto t0 = clock_type::now();
    m->fit(rollout);
    e.fit_s = secs_since(t0);
    e.fit_metric = m->last_fit_metric();
    evals[m->name()] = e;
  }

  // 3) replay the truth world (same seed) and score predictions
  WorldState ws;
  TruthWorld truth;
  truth.init(cfg.seed, cfg.n_bodies, ws);

  std::map<std::string, std::vector<double>> predict_us;
  for (int t = 0; t < cfg.ticks; ++t) {
    // current visible entity state at tick t
    std::vector<ModelInputEntity> inputs;
    inputs.reserve(truth.bodies().size());
    for (const TruthBody& b : truth.bodies()) {
      inputs.push_back(ModelInputEntity{b.id, b.class_index, b.pos, b.vel});
    }

    // each model predicts t+1 BEFORE the world steps (no lookahead)
    std::map<std::string, std::vector<Proposal>> predictions;
    for (auto& m : models) {
      const auto t1 = clock_type::now();
      predictions[m->name()] =
          m->predict(truth.tick(), inputs, cfg.model_confidence);
      predict_us[m->name()].push_back(micros_since(t1));
    }

    // the world transitions t -> t+1 (the ground truth)
    truth.step(ws);

    // score: prediction vs actual transition, matched by target_hint
    for (auto& m : models) {
      ModelEval& e = evals[m->name()];
      for (const Proposal& p : predictions[m->name()]) {
        if (!p.position.has_value() || !p.target_hint.has_value()) continue;
        const TruthBody* actual = nullptr;
        for (const TruthBody& b : truth.bodies()) {
          if (b.id == *p.target_hint) {
            actual = &b;
            break;
          }
        }
        if (!actual) continue;
        const double dx = double(p.position->x) - double(actual->pos.x);
        const double dz = double(p.position->z) - double(actual->pos.y);
        const double d = std::sqrt(dx * dx + dz * dz);
        e.mean_divergence += d;
        if (d > e.max_divergence) e.max_divergence = d;
        ++e.n_predictions;
      }
    }
  }

  for (auto& [name, e] : evals) {
    if (e.n_predictions > 0) e.mean_divergence /= double(e.n_predictions);
    e.predict_p50_us = percentile(predict_us[name], 0.50);
    e.predict_p95_us = percentile(predict_us[name], 0.95);
    res.models.push_back(e);  // map iteration => sorted by name
  }
  return res;
}

}  // namespace ow
