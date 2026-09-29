#pragma once
// Grounding Loop v0 — the closed loop being measured:
//   truth step -> render Observation -> perception adapters ->
//   runtime adjudication (identity/ownership) -> belief log -> metrics.
// Everything is timed per stage; everything is deterministic for a seed.
#include "perception/adapter.hpp"

#include <cmath>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ow {

struct LoopConfig {
  std::uint64_t seed = 42;
  int n_bodies = 12;
  int ticks = 600;
  float assoc_threshold = 0.06f;
  bool collect_per_tick_digests = false;
};

struct ProducerMetrics {
  std::string producer;
  long n_proposals = 0;
  long n_applied = 0;
  long n_rejected = 0;
  long n_assoc_correct = 0;  // applied AND adjudicated id == true source id
  double sum_transform_error = 0.0;
  long n_err_samples = 0;
  long n_high_conf = 0;       // confidence >= 0.7
  long n_high_conf_wrong = 0;  // high-conf AND (wrong id OR error > 0.05)

  // Expected Calibration Error bins (5 equal bins over [0,1]) over applied
  // proposals: |mean confidence - empirical correctness| per bin, weighted.
  // Catches BOTH over-confident and under-confident miscalibration — the
  // high-conf-wrong-rate alone has a blind spot for the inverted model.
  long bin_n[5] = {0, 0, 0, 0, 0};
  long bin_good[5] = {0, 0, 0, 0, 0};
  double bin_conf_sum[5] = {0, 0, 0, 0, 0};

  double ece() const {
    long total = 0;
    for (int b = 0; b < 5; ++b) total += bin_n[b];
    if (total == 0) return 0.0;
    double e = 0.0;
    for (int b = 0; b < 5; ++b) {
      if (bin_n[b] == 0) continue;
      const double conf_mean = bin_conf_sum[b] / double(bin_n[b]);
      const double acc = double(bin_good[b]) / double(bin_n[b]);
      e += (double(bin_n[b]) / double(total)) * std::fabs(conf_mean - acc);
    }
    return e;
  }

  double identity_stability() const {
    return n_applied ? double(n_assoc_correct) / double(n_applied) : 0.0;
  }
  double mean_transform_error() const {
    return n_err_samples ? sum_transform_error / double(n_err_samples) : 0.0;
  }
  double high_conf_wrong_rate() const {
    return n_high_conf ? double(n_high_conf_wrong) / double(n_high_conf) : 0.0;
  }
  double applied_rate() const {
    return n_proposals ? double(n_applied) / double(n_proposals) : 0.0;
  }
};

struct LoopResult {
  std::uint64_t seed = 0;
  int n_bodies = 0;
  int ticks = 0;
  double total_wall_s = 0.0;
  double loop_hz = 0.0;
  std::uint64_t final_state_digest = 0;
  std::uint64_t final_image_digest = 0;
  std::vector<std::uint64_t> per_tick_state_digests;
  std::map<std::string, std::vector<double>> stage_us;  // per stage samples
  std::vector<ProducerMetrics> producers;               // sorted by name
  std::vector<std::string> adapters_used;               // sorted
};

// Runs the loop over ALL registered perception adapters (sorted names).
LoopResult run_grounding_loop(const LoopConfig& cfg);

}  // namespace ow
