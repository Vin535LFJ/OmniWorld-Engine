#pragma once
// Perception seam (contract: FR6 adapter invariant). An adapter sees ONLY the
// Observation and emits ONLY Proposals — notice this header includes no
// world_state: the seam is structurally truth-blind.
// Registration is single-file self-registration (OW_REGISTER_PERCEPTION);
// the registry lookup is the core's only per-adapter branch point.
#include "core/proposal.hpp"
#include "observation/observation.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ow {

class PerceptionAdapter {
 public:
  virtual ~PerceptionAdapter() = default;
  virtual std::vector<Proposal> observe(const Observation&) = 0;
  virtual const char* name() const = 0;
};

namespace perception {

using Factory = std::unique_ptr<PerceptionAdapter> (*)();

bool reg(const char* name, Factory f);                       // self-register
std::unique_ptr<PerceptionAdapter> make(const std::string& name);  // nullptr if unknown
std::vector<std::string> names();  // sorted names (deterministic enumeration)

}  // namespace perception
}  // namespace ow

// Usage: at namespace scope in the adapter's own single file:
//   OW_REGISTER_PERCEPTION("my_detector", MyDetector)
#define OW_REGISTER_PERCEPTION(ADAPTER_NAME, TYPE)                 \
  static std::unique_ptr<::ow::PerceptionAdapter> make_##TYPE() {  \
    return std::make_unique<TYPE>();                               \
  }                                                                \
  static const bool g_reg_##TYPE =                                 \
      ::ow::perception::reg(ADAPTER_NAME, &make_##TYPE);
