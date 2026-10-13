#include "world_state/world_state.hpp"

#include <utility>

namespace ow {

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
  }
  ++tick_;
}

}  // namespace ow
