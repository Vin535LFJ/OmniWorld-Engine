#include "tolerance/tolerance.hpp"

#include <cmath>

namespace ow {

ToleranceVerdict compare_within_tolerance(
    const std::map<std::string, double>& a,
    const std::map<std::string, double>& b,
    const std::vector<ToleranceWindow>& windows) {
  ToleranceVerdict verdict;
  verdict.pass = true;
  for (const ToleranceWindow& w : windows) {
    const auto ia = a.find(w.metric);
    const auto ib = b.find(w.metric);
    if (ia == a.end() || ib == b.end()) {
      verdict.deltas.emplace_back(w.metric, kMissingDelta);
      verdict.pass = false;
      continue;
    }
    const double delta = std::abs(ia->second - ib->second);
    verdict.deltas.emplace_back(w.metric, delta);
    if (!(delta <= w.abs_tol)) verdict.pass = false;
  }
  return verdict;
}

}  // namespace ow
