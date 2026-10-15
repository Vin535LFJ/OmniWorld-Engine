// omniworld_loop — stage-A closed loop CLI (reference mode, byte-exact).
// Example: omniworld_loop --ticks 2000 --seed 7 --adapters blob_detect
#include "loop/driver.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace {

std::vector<std::string> split_csv(const char* s) {
  std::vector<std::string> out;
  std::string cur;
  for (const char* p = s; *p != '\0'; ++p) {
    if (*p == ',') {
      out.push_back(cur);
      cur.clear();
    } else {
      cur.push_back(*p);
    }
  }
  out.push_back(cur);
  return out;
}

}  // namespace

int main(int argc, char** argv) {
  ow::RunConfig cfg{};
  cfg.ticks = 2000;
  cfg.seed = 7;
  cfg.width = 64;
  cfg.height = 64;
  cfg.footprint = 0.02f;
  cfg.perception_adapters = {"blob_detect", "blob_detect_overconfident"};
  cfg.world_models = {"zero_delta", "linear_extrapolation"};

  for (int i = 1; i < argc; ++i) {
    const char* a = argv[i];
    if (std::strcmp(a, "--ticks") == 0 && i + 1 < argc) {
      cfg.ticks = static_cast<ow::Tick>(std::strtoull(argv[++i], nullptr, 10));
    } else if (std::strcmp(a, "--seed") == 0 && i + 1 < argc) {
      cfg.seed = std::strtoull(argv[++i], nullptr, 10);
    } else if (std::strcmp(a, "--width") == 0 && i + 1 < argc) {
      cfg.width = std::atoi(argv[++i]);
    } else if (std::strcmp(a, "--height") == 0 && i + 1 < argc) {
      cfg.height = std::atoi(argv[++i]);
    } else if (std::strcmp(a, "--footprint") == 0 && i + 1 < argc) {
      cfg.footprint = static_cast<float>(std::atof(argv[++i]));
    } else if (std::strcmp(a, "--adapters") == 0 && i + 1 < argc) {
      cfg.perception_adapters = split_csv(argv[++i]);
    } else if (std::strcmp(a, "--models") == 0 && i + 1 < argc) {
      cfg.world_models = split_csv(argv[++i]);
    } else {
      std::printf(
          "usage: omniworld_loop [--ticks N] [--seed S] [--width W] "
          "[--height H] [--footprint F] [--adapters a,b] [--models m,n]\n");
      return 2;
    }
  }

  ow::LoopDriver loop(8);
  const ow::RunReport r = loop.run(cfg);
  std::printf("ticks=%llu seconds=%.6f hz=%.1f\n",
              static_cast<unsigned long long>(r.ticks_run), r.total_seconds,
              r.hz);
  std::printf("world_hash=%016llx ledger_hash=%016llx entries=%zu\n",
              static_cast<unsigned long long>(r.world_hash),
              static_cast<unsigned long long>(r.ledger_hash),
              r.ledger_entries);
  std::printf("applied=%zu announced=%zu rejected=%zu faults=%zu\n",
              r.applied, r.announced, r.rejected, r.adapter_faults);
  std::printf("divergence=%.6f ece=%.6f identity_stability=%.6f\n",
              r.metrics.at("divergence"), r.metrics.at("ece"),
              r.metrics.at("identity_stability"));
  std::printf("latency_ms: render=%.3f perceive=%.3f adjudicate=%.3f metrics=%.3f\n",
              r.latency_sum.render_ms, r.latency_sum.perceive_ms,
              r.latency_sum.adjudicate_ms, r.latency_sum.metrics_ms);
  return 0;
}
