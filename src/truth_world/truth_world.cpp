#include "truth_world/truth_world.hpp"

#include "perception/world_palette.hpp"

namespace ow {

void TruthWorld::init(std::uint64_t seed, int n_bodies, WorldState& ws) {
  rng_ = Pcg32::seeded(seed, 0xDA3E5CBFull);
  bodies_.clear();
  bodies_.resize(static_cast<std::size_t>(n_bodies));
  n_ = n_bodies;
  tick_ = 0;

  for (int i = 0; i < n_bodies; ++i) {
    TruthBody& b = bodies_[static_cast<std::size_t>(i)];
    b.class_index = i % world::kClasses;
    b.radius = rng_.uniform_range(0.018f, 0.045f);

    // rejection-sample spawn positions with minimum separation
    const float margin = b.radius + 0.01f;
    for (int attempt = 0; attempt < 200; ++attempt) {
      Vec2 cand{rng_.uniform_range(margin, 1.0f - margin),
                rng_.uniform_range(margin, 1.0f - margin)};
      bool ok = true;
      for (const TruthBody& other : bodies_) {
        if (other.id == 0) continue;
        const float dx = cand.x - other.pos.x;
        const float dy = cand.y - other.pos.y;
        const float sep = b.radius + other.radius + 0.05f;
        if (dx * dx + dy * dy < sep * sep) {
          ok = false;
          break;
        }
      }
      if (ok) {
        b.pos = cand;
        break;
      }
    }
    const float angle = rng_.uniform_range(0.0f, 6.2831853f);
    const float speed = rng_.uniform_range(0.004f, 0.020f);
    b.vel = Vec2{std::cos(angle) * speed, std::sin(angle) * speed};
    b.id = ws.mint(world::kNames[static_cast<std::size_t>(b.class_index)]);
  }

  std::vector<TruthUpdate> updates;
  updates.reserve(bodies_.size());
  for (TruthBody& b : bodies_) {
    updates.push_back(TruthUpdate{b.id,
                                  Vec3{b.pos.x, 0.0f, b.pos.y},
                                  Vec3{b.vel.x, 0.0f, b.vel.y}});
  }
  ws.commit_truth(0, updates);
}

void TruthWorld::step(WorldState& ws) {
  ++tick_;
  std::vector<TruthUpdate> updates;
  updates.reserve(bodies_.size());
  for (TruthBody& b : bodies_) {
    b.pos = b.pos + b.vel;
    // elastic wall bounces with clamp to keep the disk inside [r, 1-r]
    const float r = b.radius;
    if (b.pos.x < r) {
      b.pos.x = r + (r - b.pos.x);
      b.vel.x = -b.vel.x;
    } else if (b.pos.x > 1.0f - r) {
      b.pos.x = (1.0f - r) - (b.pos.x - (1.0f - r));
      b.vel.x = -b.vel.x;
    }
    if (b.pos.y < r) {
      b.pos.y = r + (r - b.pos.y);
      b.vel.y = -b.vel.y;
    } else if (b.pos.y > 1.0f - r) {
      b.pos.y = (1.0f - r) - (b.pos.y - (1.0f - r));
      b.vel.y = -b.vel.y;
    }
    updates.push_back(TruthUpdate{b.id,
                                  Vec3{b.pos.x, 0.0f, b.pos.y},
                                  Vec3{b.vel.x, 0.0f, b.vel.y}});
  }
  ws.commit_truth(tick_, updates);
}

}  // namespace ow
