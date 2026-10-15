#include "adjudication/adjudication.hpp"

namespace ow {

AdjudicationResult NearestPositionStrategy::resolve(const Proposal& p,
                                                    const WorldState& ws) const {
  if (const auto bad = validate_proposal(p)) {
    return {VerdictKind::Rejected, 0, *bad};
  }
  if (const auto* id = std::get_if<EntityId>(&p.target)) {
    if (ws.find(*id) != nullptr) {
      return {VerdictKind::Matched, *id, {}};
    }
    return {VerdictKind::Rejected, 0, "unknown target id"};
  }
  // NewEntity claim: associate by nearest position in the x/z plane.
  if (!p.fields.transform) {
    return {VerdictKind::Rejected, 0, "no position claim"};
  }
  const Vec3 q = p.fields.transform->position;
  const Entity* best = nullptr;
  float best_d2 = 0.0f;
  for (const Entity& e : ws.entities()) {  // id order; first tie wins
    const float dx = e.transform.position.x - q.x;
    const float dz = e.transform.position.z - q.z;
    const float d2 = dx * dx + dz * dz;
    if (best == nullptr || d2 < best_d2) {
      best = &e;
      best_d2 = d2;
    }
  }
  if (best != nullptr && best_d2 <= tol2_) {
    return {VerdictKind::Matched, best->id, {}};
  }
  // Announced, not anchored: v1 never mints world ids outside the TruthWorld
  // path (single-writer corollary); NewEntity is a ledger-side announcement.
  return {VerdictKind::NewEntity, 0, {}};
}

}  // namespace ow
