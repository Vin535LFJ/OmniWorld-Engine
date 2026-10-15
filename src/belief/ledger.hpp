#pragma once
// Belief ledger (contract 5/6): append-only record of adjudicated proposals.
// A mistake is data, never an overwrite. Entries carry the proposal WITH the
// verdict that adjudicated it (arch-review insight #1). Tick-stamped per the
// loop's tick at recording time (recorded_at).
#include "adjudication/adjudication.hpp"
#include "core/types.hpp"

#include <vector>

namespace ow {

struct BeliefEntry {
  Proposal proposal;
  AdjudicationResult verdict;
  Tick recorded_at = 0;
};

class BeliefLedger {
 public:
  void record(BeliefEntry e) { entries_.push_back(std::move(e)); }
  const std::vector<BeliefEntry>& all() const { return entries_; }
  // Entries whose recorded_at == t, preserving append order.
  std::vector<const BeliefEntry*> at(Tick t) const {
    std::vector<const BeliefEntry*> out;
    for (const auto& e : entries_)
      if (e.recorded_at == t) out.push_back(&e);
    return out;
  }

 private:
  std::vector<BeliefEntry> entries_;
};

}  // namespace ow
