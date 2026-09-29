#pragma once
// World Contract v0 — the canonical world state (PLAN.md section 7).
// The runtime owns identity, ticks, versions, and adjudication. Truth
// owners write kinematics directly; every other writer goes through
// Proposals, which are adjudicated into the belief log (never mutating
// owned fields). Belief samples exist to be scored against truth.
#include "core/types.hpp"

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace ow {

enum class EntitySource { ground_truth, belief };

struct Entity {
  EntityId id = 0;
  std::string class_tag;
  Vec3 position;   // world frame
  Quat rotation;   // identity in v0
  Vec3 velocity;   // per-tick velocity, owned by truth
  ComponentId owner = 0;
  EntitySource source = EntitySource::ground_truth;
  Tick updated_at = 0;
};

struct TruthUpdate {
  EntityId id = 0;
  Vec3 position;
  Vec3 velocity;
};

// The only write path for non-owners.
struct Proposal {
  std::string producer;
  Tick tick_observed = 0;
  std::optional<EntityId> target_hint;  // evidence for metrics; v0 association ignores it
  std::optional<std::string> class_tag;
  std::optional<Vec3> position;
  float confidence = 0.0f;  // [0,1], producer-supplied
};

struct BeliefSample {
  Tick tick = 0;
  std::string producer;
  EntityId entity = 0;  // 0 => proposal was rejected (reason below)
  Vec3 position{};
  float confidence = 0.0f;
  const char* reason = "";  // "" if applied, else rejection code
};

struct Adjudication {
  bool applied = false;
  EntityId entity = 0;
  const char* reason = "";
};

class WorldState {
 public:
  static constexpr ComponentId kTruthOwner = 1;

  // Mint a new stable identity. Ids are never reused.
  EntityId mint(const std::string& class_tag);

  // Truth owner commits one tick of kinematics. Bumps state version.
  void commit_truth(Tick tick, const std::vector<TruthUpdate>& updates);

  // v0 association: nearest truth transform (Euclidean) with class match,
  // within assoc_threshold. Ties resolve to the smaller entity id
  // (entities iterate in id order). Applied proposals land in the belief
  // log; rejected ones are recorded with a reason. Never mutates fields.
  std::vector<Adjudication> adjudicate(std::vector<Proposal> proposals,
                                       float assoc_threshold);

  const std::map<EntityId, Entity>& entities() const { return entities_; }
  const std::vector<BeliefSample>& belief_log() const { return belief_; }

  std::uint64_t version() const { return version_; }
  Tick tick() const { return tick_; }

  // Deterministic digest over ordered entity fields + version/tick.
  std::uint64_t state_digest() const;

  EntityId next_id_preview() const { return next_id_; }

 private:
  std::map<EntityId, Entity> entities_;  // ordered => deterministic iteration
  EntityId next_id_ = 1;
  std::uint64_t version_ = 0;
  Tick tick_ = 0;
  std::vector<BeliefSample> belief_;
};

}  // namespace ow
