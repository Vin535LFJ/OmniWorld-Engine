#pragma once
// Phase 1 evaluation loop — world models in the closed loop:
//   record rollout -> fit(rollout) each model -> replay the truth world
//   -> at every tick t each model predicts t+1 -> step -> score prediction
//   vs the actual truth transition (prediction-vs-truth divergence).
#include "core/types.hpp"

#include <map>
#include <string>
#include <vector>

namespace ow {

struct Phase1Config {
  std::uint64_t seed = 42;
  int n_bodies = 12;
  int ticks = 600;
  // fixed proposal confidence for world models in v0.1 evaluation
  // (per-model confidence estimation is deferred, documented)
  float model_confidence = 0.5f;
};

struct ModelEval {
  std::string model;
  long n_predictions = 0;
  double mean_divergence = 0.0;  // mean |pred - actual| per prediction
  double max_divergence = 0.0;
  double fit_s = 0.0;            // time spent in fit()
  double fit_metric = 0.0;        // model-specific (e.g., train loss)
  double predict_p50_us = 0.0;
  double predict_p95_us = 0.0;
};

struct Phase1Result {
  std::uint64_t seed = 0;
  int n_bodies = 0;
  int ticks = 0;
  std::uint64_t rollout_digest = 0;
  std::vector<ModelEval> models;  // sorted by model name
};

Phase1Result run_world_model_eval(const Phase1Config& cfg);

}  // namespace ow
