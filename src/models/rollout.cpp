#include "models/rollout.hpp"

#include "truth_world/truth_world.hpp"
#include "world_state/world_state.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>

namespace ow {

Rollout record_rollout(std::uint64_t seed, int n_bodies, int ticks) {
  Rollout r;
  r.seed = seed;
  r.n_bodies = n_bodies;
  r.ticks.reserve(static_cast<std::size_t>(ticks) + 1);

  WorldState ws;
  TruthWorld truth;
  truth.init(seed, n_bodies, ws);

  auto capture = [&](TruthWorld& tw, WorldState& w) {
    RolloutTick rt;
    rt.tick = tw.tick();
    for (const TruthBody& b : tw.bodies()) {
      rt.entities.push_back(
          RolloutEntity{b.id, b.class_index, b.pos, b.vel});
    }
    std::sort(rt.entities.begin(), rt.entities.end(),
              [](const RolloutEntity& a, const RolloutEntity& b) {
                return a.id < b.id;
              });
    r.ticks.push_back(std::move(rt));
  };

  capture(truth, ws);
  for (int t = 0; t < ticks; ++t) {
    truth.step(ws);
    capture(truth, ws);
  }
  return r;
}

bool write_rollout_text(const Rollout& r, const std::string& path) {
  std::ofstream f(path, std::ios::binary);
  if (!f) return false;
  f << "# omniworld rollout v1: tick id class x z vx vz (tab-separated)\n";
  f << "# seed " << r.seed << " bodies " << r.n_bodies << " ticks "
    << r.ticks.size() << "\n";
  for (const RolloutTick& t : r.ticks) {
    for (const RolloutEntity& e : t.entities) {
      f << t.tick << '\t' << e.id << '\t' << e.class_index << '\t'
        << e.pos.x << '\t' << e.pos.y << '\t' << e.vel.x << '\t' << e.vel.y
        << '\n';
    }
  }
  return true;
}

}  // namespace ow
