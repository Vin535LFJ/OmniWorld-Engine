#include "models/world_model.hpp"

// Zero-delta model — the "world is static" floor baseline: predicts the
// position never changes. Should be the WORST model on a moving world;
// any model scoring below it is not learning anything.
namespace ow {
namespace {

class ZeroModel final : public WorldModelAdapter {
 public:
  std::string name() const override { return "zero_delta"; }

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
      p.position = Vec3{e.pos.x, 0.0f, e.pos.y};
      p.confidence = confidence;
      out.push_back(std::move(p));
    }
    return out;
  }
};

}  // namespace

OW_REGISTER_WORLD_MODEL(
    "zero_delta", [] { return std::unique_ptr<WorldModelAdapter>(new ZeroModel()); });

}  // namespace ow
