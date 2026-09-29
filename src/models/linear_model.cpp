#include "models/world_model.hpp"

// Linear-extrapolation model — a classical dynamics baseline: predicts
// next position as pos + vel. Exact for free flight, wrong at wall
// bounces. Added as ONE self-registering file with zero core edits —
// the Phase-1 "new model joins via one adapter file" proof.
namespace ow {
namespace {

class LinearModel final : public WorldModelAdapter {
 public:
  std::string name() const override { return "linear"; }

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
      p.position = Vec3{e.pos.x + e.vel.x, 0.0f, e.pos.y + e.vel.y};
      p.confidence = confidence;
      out.push_back(std::move(p));
    }
    return out;
  }
};

}  // namespace

OW_REGISTER_WORLD_MODEL(
    "linear", [] { return std::unique_ptr<WorldModelAdapter>(new LinearModel()); });

}  // namespace ow
