#pragma once
// World-model adapter contract (Phase 1): a "world model" predicts the
// world's next state. Given the current entity state it returns proposals
// carrying PREDICTED next-tick positions, one per entity, with target_hint
// naming the entity being predicted (evidence used by the divergence
// metric; the runtime's association rule stays unchanged).
// Adding a model = adding one .cpp with OW_REGISTER_WORLD_MODEL — zero
// core edits (Phase-1 exit invariance, mirroring Gate 1b).
#include "core/types.hpp"
#include "world_state/world_state.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ow {

// A world-model adapter is state-conditioned: input entities, output predictions.
struct ModelInputEntity {
  EntityId id = 0;
  int class_index = 0;
  Vec2 pos{};  // plane position at tick t
  Vec2 vel{};  // per-tick velocity at tick t
};

class Rollout;  // models/rollout.hpp

class WorldModelAdapter {
 public:
  virtual ~WorldModelAdapter() = default;
  virtual std::string name() const = 0;
  // Optional training/calibration from a recorded rollout (deterministic).
  virtual void fit(const Rollout& rollout) {}
  // Optional diagnostic from the last fit (e.g., final training loss).
  virtual double last_fit_metric() const { return 0.0; }
  // Predict each entity's position at tick+1.
  virtual std::vector<Proposal> predict(Tick current,
                                        const std::vector<ModelInputEntity>& entities,
                                        float confidence) const = 0;
};

namespace detail {
std::map<std::string, std::function<std::unique_ptr<WorldModelAdapter>()>>&
world_model_registry();
bool register_world_model(
    const std::string& name,
    std::function<std::unique_ptr<WorldModelAdapter>()> factory);
}  // namespace detail

std::vector<std::string> world_model_names();
std::unique_ptr<WorldModelAdapter> create_world_model(const std::string& name);
std::vector<std::unique_ptr<WorldModelAdapter>> create_all_world_models();

}  // namespace ow

#define OW_REGISTER_WORLD_MODEL(NAME, FACTORY)                     \
  static const bool ow_world_model_reg_##__COUNTER__ =             \
      ::ow::detail::register_world_model(NAME, FACTORY)
