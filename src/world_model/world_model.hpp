#pragma once
// World-model seam: the second adapter family. A world model SEES the
// canonical state (unlike perception) and predicts the next state as
// Proposals; its proposals flow through the SAME adjudication path.
// Single-file self-registration, mirroring the perception seam.
#include "core/proposal.hpp"
#include "world_state/world_state.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ow {

class WorldModelAdapter {
 public:
  virtual ~WorldModelAdapter() = default;
  virtual std::vector<Proposal> predict(Tick, const WorldState&) = 0;
  virtual const char* name() const = 0;
};

namespace world_model {

using Factory = std::unique_ptr<WorldModelAdapter> (*)();

bool reg(const char* name, Factory f);
std::unique_ptr<WorldModelAdapter> make(const std::string& name);
std::vector<std::string> names();  // sorted

}  // namespace world_model
}  // namespace ow

// TYPE must be a BARE identifier visible in the enclosing namespace
// (problem-log 004: token-paste does not eat qualified names).
#define OW_REGISTER_WORLD_MODEL(MODEL_NAME, TYPE)                  \
  static std::unique_ptr<::ow::WorldModelAdapter> make_##TYPE() {  \
    return std::make_unique<TYPE>();                               \
  }                                                                \
  static const bool g_reg_##TYPE =                                 \
      ::ow::world_model::reg(MODEL_NAME, &make_##TYPE);
