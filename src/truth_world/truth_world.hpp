#pragma once
// Deterministic seeded kinematic truth world: N disks moving on the
// [0,1]^2 ground plane with elastic wall bounces. No inter-body contacts
// in v0 (documented: kinematic truth, not contact simulation). The truth
// world is the sole owner of kinematics fields in WorldState.
#include "core/rng.hpp"
#include "core/types.hpp"
#include "world_state/world_state.hpp"

#include <vector>

namespace ow {

struct TruthBody {
  EntityId id = 0;
  int class_index = 0;
  Vec2 pos{};   // on plane [0,1]^2, y=0 in world frame
  Vec2 vel{};   // per-tick velocity
  float radius = 0.03f;
};

class TruthWorld {
 public:
  void init(std::uint64_t seed, int n_bodies, WorldState& ws);
  void step(WorldState& ws);  // advances one tick and commits truth

  const std::vector<TruthBody>& bodies() const { return bodies_; }
  Tick tick() const { return tick_; }

 private:
  std::vector<TruthBody> bodies_;
  Pcg32 rng_{};
  Tick tick_ = 0;
  int n_ = 0;
};

}  // namespace ow
