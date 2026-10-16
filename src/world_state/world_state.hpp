#pragma once
// Canonical world state. The public surface is READ-ONLY except the derived
// boundary-event counter; the only writer is TruthWorld (contract 4/5,
// structural single-writer): all mutation is private and reachable solely
// through the friend channel below. A future second writer must amend this
// one visible place and go through an ADR.
#include "core/types.hpp"

#include <vector>

namespace ow {

class TruthWorld;  // the single writer

struct Entity {
  EntityId id = 0;  // minted by the runtime; never reused
  ClassTag class_tag;
  Transform transform;
  Vec3 velocity;
};

class WorldState {
 public:
  explicit WorldState(BoundsPolicy bounds);  // the only injection point

  Tick tick() const { return tick_; }
  std::size_t size() const { return entities_.size(); }
  const Entity* find(EntityId id) const;
  const std::vector<Entity>& entities() const { return entities_; }
  // Cumulative wall reflections since construction (diagnostic; contract 9).
  std::size_t boundary_events() const { return boundary_events_; }

 private:
  friend class TruthWorld;

  EntityId mint(ClassTag tag, Transform t, Vec3 v);
  void step();  // constant-velocity integration, dt = 1 tick, id order,
                // then bounded reflection per BoundsPolicy (x/z axes)

  Tick tick_ = 0;
  EntityId next_id_ = 1;          // 0 is invalid; monotonic; never reused
  std::vector<Entity> entities_;  // stable id order — iteration order is part
                                  // of determinism
  BoundsPolicy bounds_;
  std::size_t boundary_events_ = 0;
};

}  // namespace ow
