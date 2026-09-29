#include "models/rollout.hpp"
#include "models/world_model.hpp"

// Replay model — the oracle baseline: returns the recorded next-tick
// position for each entity. Its divergence must be exactly 0; any nonzero
// value would expose a bug in the evaluation pipeline, which is the point
// of having an oracle in the harness.
namespace ow {
namespace {

class ReplayModel final : public WorldModelAdapter {
 public:
  std::string name() const override { return "replay"; }

  void fit(const Rollout& rollout) override { rollout_ = &rollout; }

  std::vector<Proposal> predict(
      Tick current, const std::vector<ModelInputEntity>& entities,
      float confidence) const override {
    std::vector<Proposal> out;
    if (!rollout_ || current + 1 >= rollout_->ticks.size()) return out;
    const RolloutTick& next = rollout_->ticks[static_cast<std::size_t>(current + 1)];
    for (const ModelInputEntity& e : entities) {
      const RolloutEntity* rec = nullptr;
      for (const RolloutEntity& cand : next.entities) {
        if (cand.id == e.id) {
          rec = &cand;
          break;
        }
      }
      if (!rec) continue;
      Proposal p;
      p.producer = name();
      p.tick_observed = current;
      p.target_hint = e.id;
      p.position = Vec3{rec->pos.x, 0.0f, rec->pos.y};
      p.confidence = confidence;
      out.push_back(std::move(p));
    }
    return out;
  }

 private:
  const Rollout* rollout_ = nullptr;
};

}  // namespace

OW_REGISTER_WORLD_MODEL(
    "replay", [] { return std::unique_ptr<WorldModelAdapter>(new ReplayModel()); });

}  // namespace ow
