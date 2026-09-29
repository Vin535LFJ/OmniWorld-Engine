#include "models/world_model.hpp"

// Damped-linear model — the Phase-1 adapter-only invariant demonstration:
// this file is the ONLY change in its commit (no core/loop/test edits);
// the evaluation loop discovers it purely via self-registration and it
// appears in the next benchmark run automatically. Predicts pos + 0.5*vel.
namespace ow {
namespace {

class DampedLinearModel final : public WorldModelAdapter {
 public:
  std::string name() const override { return "damped_linear"; }

  std::vector<Proposal> predict(
      Tick current, const std::vector<ModelInputEntity>& entities,
      float confidence) const override {
    std::vector<Proposal> out;
    out.reserve(entities.size());
    for (const ModelInputEntity& e : entities) {
      Proposal p;
      p.producer = name();
      p.tick_observed = current;
      p.target_hint = e.id;
      p.position =
          Vec3{e.pos.x + 0.5f * e.vel.x, 0.0f, e.pos.y + 0.5f * e.vel.y};
      p.confidence = confidence;
      out.push_back(std::move(p));
    }
    return out;
  }
};

}  // namespace

OW_REGISTER_WORLD_MODEL("damped_linear", [] {
  return std::unique_ptr<WorldModelAdapter>(new DampedLinearModel());
});

}  // namespace ow
