#pragma once
// Perception adapter contract. An adapter sees ONLY the RGB image (never
// the id map) and returns confidence-annotated world-frame proposals.
// Adding a model = adding one .cpp with OW_REGISTER_PERCEPTION — zero core
// edits (Gate 1b / Phase-1 exit invariance).
#include "observation/raster.hpp"
#include "world_state/world_state.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ow {

class PerceptionAdapter {
 public:
  virtual ~PerceptionAdapter() = default;
  virtual std::string name() const = 0;
  virtual std::vector<Proposal> detect(const Image& img,
                                       const TopDownCamera& cam,
                                       Tick observed) const = 0;
};

namespace detail {
std::map<std::string, std::function<std::unique_ptr<PerceptionAdapter>()>>&
perception_registry();
bool register_perception(
    const std::string& name,
    std::function<std::unique_ptr<PerceptionAdapter>()> factory);
}  // namespace detail

// names in sorted order (deterministic)
std::vector<std::string> perception_names();
std::unique_ptr<PerceptionAdapter> create_perception(const std::string& name);
std::vector<std::unique_ptr<PerceptionAdapter>> create_all_perception();

}  // namespace ow

#define OW_REGISTER_PERCEPTION(NAME, FACTORY)                            \
  static const bool ow_perception_reg_##__COUNTER__ =                    \
      ::ow::detail::register_perception(NAME, FACTORY)
