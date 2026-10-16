#pragma once
// Tolerance comparator (stage B slice B2): ADR-0009's tolerance mechanism as
// a pure, stateless function — no I/O, no clock, no RNG. Windows are declared
// by metric key (decomposed keys such as "ece:<producer>" are plain strings).
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace ow {

struct ToleranceWindow {
  std::string metric;   // key to compare (any metric-map key)
  double abs_tol;       // inclusive upper bound on |a-b|
};

struct ToleranceVerdict {
  bool pass;
  // All compared keys' |a-b|; a key missing on either side reports delta
  // kMissingDelta (-1.0) and forces pass=false (documented sentinel).
  std::vector<std::pair<std::string, double>> deltas;
};

/** Sentinel delta for a window key missing on either side. */
constexpr double kMissingDelta = -1.0;

ToleranceVerdict compare_within_tolerance(
    const std::map<std::string, double>& a,
    const std::map<std::string, double>& b,
    const std::vector<ToleranceWindow>& windows);

}  // namespace ow
