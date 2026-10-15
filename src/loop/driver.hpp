#pragma once
// LoopDriver — the orchestrator (L0 sec 3.2 sequence diagram is the authority
// for the order). It is the only component that knows every other one; all
// cross-module dependencies flow into it. Wall-clock latencies are diagnostic
// only (contract 3) and never enter any hash.
#include "core/types.hpp"

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ow {

struct StageLatency {
  double render_ms = 0.0;
  double perceive_ms = 0.0;
  double adjudicate_ms = 0.0;
  double metrics_ms = 0.0;
};

struct TickLog {
  Tick tick = 0;
  std::size_t applied = 0;    // Matched
  std::size_t announced = 0;  // NewEntity
  std::size_t rejected = 0;   // Rejected (with reason, in the ledger)
  StageLatency latency;
};

struct RunConfig {
  Tick ticks = 0;
  std::uint64_t seed = 0;
  int width = 0;   // no defaults: the caller must be explicit
  int height = 0;
  float footprint = 0.0f;
  std::vector<std::string> perception_adapters;  // registry names
  std::vector<std::string> world_models;         // registry names
};

struct RunReport {
  Tick ticks_run = 0;
  double total_seconds = 0.0;  // diagnostic
  double hz = 0.0;             // diagnostic (wall clock)
  std::map<std::string, double> metrics;
  std::uint64_t world_hash = 0;   // deterministic byte-level identity
  std::uint64_t ledger_hash = 0;  // deterministic
  std::size_t ledger_entries = 0;
  std::size_t applied = 0, announced = 0, rejected = 0;
  StageLatency latency_sum;
  std::size_t adapter_faults = 0;  // caught exceptions (isolation proof)
};

class LoopDriver {
 public:
  explicit LoopDriver(int scene_entities = 8) : scene_entities_(scene_entities) {}
  RunReport run(const RunConfig&);

 private:
  int scene_entities_;
};

}  // namespace ow
