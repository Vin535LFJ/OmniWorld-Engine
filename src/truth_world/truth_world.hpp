#pragma once
// Truth World: deterministic kinematics; the single writer of WorldState.
#include "core/rng.hpp"
#include "core/types.hpp"
#include "world_state/world_state.hpp"

namespace ow {

class TruthWorld {
 public:
  explicit TruthWorld(std::uint64_t seed) : rng_(seed) {}

  EntityId spawn(WorldState& ws, ClassTag tag, Transform t, Vec3 velocity);
  void advance(WorldState& ws, Tick target);  // step until tick == target
  void build_demo_scene(WorldState& ws, int n);

 private:
  Rng rng_;
};

}  // namespace ow
