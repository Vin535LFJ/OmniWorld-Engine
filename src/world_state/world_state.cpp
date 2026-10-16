#include "world_state/world_state.hpp"

#include <utility>

namespace ow {
namespace {

// Mirror-reflect one axis at most once per tick; clamp when a single
// reflection still lands outside (extreme speeds). Counts one event per
// reflection actually performed (clamp after reflection adds no event).
void reflect_axis(float& pos, float& vel, float margin,
                  std::size_t& events) {
  const float lo = margin;
  const float hi = 1.0f - margin;
  if (pos < lo) {
    pos = lo + (lo - pos);
    vel = -vel;
    ++events;
  } else if (pos > hi) {
    pos = hi - (pos - hi);
    vel = -vel;
    ++events;
  }
  if (pos < lo) pos = lo;
  if (pos > hi) pos = hi;
}

}  // namespace

WorldState::WorldState(BoundsPolicy bounds) : bounds_(bounds) {}

const Entity* WorldState::find(EntityId id) const {
  for (const Entity& e : entities_) {
    if (e.id == id) {
      return &e;
    }
  }
  return nullptr;
}

EntityId WorldState::mint(ClassTag tag, Transform t, Vec3 v) {
  Entity e;
  e.id = next_id_++;
  e.class_tag = std::move(tag);
  e.transform = t;
  e.velocity = v;
  entities_.push_back(std::move(e));
  return entities_.back().id;
}

void WorldState::step() {
  for (Entity& e : entities_) {  // vector order == id order == deterministic
    e.transform.position.x += e.velocity.x;
    e.transform.position.y += e.velocity.y;
    e.transform.position.z += e.velocity.z;
    if (bounds_.kind == BoundsPolicy::Kind::Wall) {
      reflect_axis(e.transform.position.x, e.velocity.x, bounds_.margin,
                   boundary_events_);
      reflect_axis(e.transform.position.z, e.velocity.z, bounds_.margin,
                   boundary_events_);
    }
  }
  ++tick_;
}

}  // namespace ow
