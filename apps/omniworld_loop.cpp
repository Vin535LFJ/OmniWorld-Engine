// omniworld_loop — stage-A closed loop CLI (reference mode, byte-exact).
// Stage B: --mode reference|performance; --out writes the deterministic JSON
// run report (mode + environment metadata, per ADR-0009).
#include "loop/driver.hpp"

#include <cstdio>
#include <cstring>
#include <string>

int main(int argc, char** argv) {
  std::string artifact_path;
  const auto parsed = ::ow::parse_config(argc, argv, artifact_path);
  if (const std::string* err = std::get_if<std::string>(&parsed)) {
    std::fprintf(stderr, "error: %s\n", err->c_str());
    std::fprintf(stderr,
                 "usage: omniworld_loop [--ticks N] [--seed S] [--width W] "
                 "[--height H] [--footprint F] [--adapters a,b] [--models m,n] "
                 "[--mode reference|performance] [--out path]\n");
    return 2;
  }
  const ::ow::RunConfig cfg = std::get<::ow::RunConfig>(parsed);

  ::ow::LoopDriver loop(8);
  const ::ow::RunReport r = loop.run(cfg);
  const std::string json = ::ow::run_report_json(r);
  if (!artifact_path.empty()) {
    if (FILE* f = std::fopen(artifact_path.c_str(), "wb")) {
      std::fwrite(json.data(), 1, json.size(), f);
      std::fclose(f);
      std::printf("artifact: %s\n", artifact_path.c_str());
    } else {
      std::fprintf(stderr, "error: cannot write %s\n", artifact_path.c_str());
      return 2;
    }
  }

  std::printf("mode=%s\n", r.mode == ::ow::RunMode::Reference ? "reference" : "performance");
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
