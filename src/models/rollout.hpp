#pragma once
// Rollout recording: the truth world's per-tick entity states, used as
// (a) the replay model's data, (b) the MLP's training set, (c) an artifact.
#include "core/types.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace ow {

struct RolloutEntity {
  EntityId id = 0;
  int class_index = 0;
  Vec2 pos{};
  Vec2 vel{};
};

struct RolloutTick {
  Tick tick = 0;
  std::vector<RolloutEntity> entities;  // sorted by entity id
};

struct Rollout {
  std::uint64_t seed = 0;
  int n_bodies = 0;
  std::vector<RolloutTick> ticks;  // contiguous, ticks[t].tick == t
};

// Deterministic recording: identical seed => bit-identical rollout.
Rollout record_rollout(std::uint64_t seed, int n_bodies, int ticks);

// Text artifact (tab-separated: tick id class x z vx vz).
bool write_rollout_text(const Rollout& r, const std::string& path);

}  // namespace ow
