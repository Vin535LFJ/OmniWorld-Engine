#include "truth_world/truth_world.hpp"

#include <utility>

namespace ow {

EntityId TruthWorld::spawn(WorldState& ws, ClassTag tag, Transform t,
                           Vec3 velocity) {
  return ws.mint(std::move(tag), t, velocity);
}

void TruthWorld::advance(WorldState& ws, Tick target) {
  while (ws.tick() < target) {
    ws.step();
  }
  // target < current tick: defensive no-op (L2 sec 5).
}

void TruthWorld::build_demo_scene(WorldState& ws, int n) {
  for (int i = 0; i < n; ++i) {
    Transform t;
    t.position = Vec3{rng_.next_unit(), 0.0f, rng_.next_unit()};
    const Vec3 v{(rng_.next_unit() - 0.5f) * 0.1f, 0.0f,
                 (rng_.next_unit() - 0.5f) * 0.1f};
    spawn(ws, ClassTag{"target"}, t, v);
  }
}

}  // namespace ow
