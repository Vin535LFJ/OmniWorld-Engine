#pragma once
// Adjudication (contract 2 & 5): a replaceable association strategy resolves
// every Proposal to Matched / NewEntity / Rejected-with-reason. The result is
// recorded together with the proposal it adjudicated (belief/ledger.hpp) —
// arch-review insight #1 baked into the data model.
#include "core/proposal.hpp"
#include "core/types.hpp"
#include "world_state/world_state.hpp"

#include <string>

namespace ow {

enum class VerdictKind { Matched, NewEntity, Rejected };

struct AdjudicationResult {
  VerdictKind kind = VerdictKind::Rejected;
  EntityId matched = 0;    // truth-side entity id; 0 = none
  std::string reason;      // populated on Rejected
};

class AssociationStrategy {
 public:
  virtual ~AssociationStrategy() = default;
  virtual AdjudicationResult resolve(const Proposal&, const WorldState&) const = 0;
};

// v1 default: nearest neighbour by squared distance in the x/z plane,
// tolerance-gated. Deterministic: fixed iteration order, first-tie wins.
class NearestPositionStrategy final : public AssociationStrategy {
 public:
  explicit NearestPositionStrategy(float tolerance = 0.1f)
      : tol2_(tolerance * tolerance) {}
  AdjudicationResult resolve(const Proposal&, const WorldState&) const override;

 private:
  float tol2_;
};

}  // namespace ow
