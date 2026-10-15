#pragma once
// Proposal — the ONLY write path for non-owners (contract 5). Mandatory
// five-tuple: who / when-seen / which-entity / what-claim / how-sure.
// Confidence is a self-reported scalar in [0,1] with zero adjudication power
// (contract 7); its only consumer is calibration metrics.
#include "core/types.hpp"

#include <optional>
#include <string>
#include <variant>

namespace ow {

struct NewEntity {};  // explicit "I claim something not yet in the world"

struct ProposedFields {
  std::optional<Transform> transform;   // claimed pose (may be absent)
  std::optional<ClassTag> class_tag;    // OPTIONAL: adapters must not guess
};

struct Proposal {
  std::string producer;
  Tick tick_observed = 0;
  std::variant<EntityId, NewEntity> target = NewEntity{};
  ProposedFields fields;
  float confidence = 0.0f;
};

// Schema check used at intake (adjudication, slice 04). Returns the rejection
// reason, or nullopt if well-formed.
inline std::optional<std::string> validate_proposal(const Proposal& p) {
  if (p.producer.empty()) return std::string{"missing producer"};
  if (!(p.confidence >= 0.0f && p.confidence <= 1.0f))
    return std::string{"confidence out of [0,1]"};
  return std::nullopt;
}

}  // namespace ow
