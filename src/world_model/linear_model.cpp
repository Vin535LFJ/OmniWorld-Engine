#include "world_model/world_model.hpp"

namespace ow {

// linear_extrapolation: pos + vel (dt = 1 tick) — the honest baseline model.
// Fixed 0.5 confidence (v0.1 as-built fact).
class LinearExtrapolation final : public WorldModelAdapter {
 public:
  const char* name() const override { return "linear_extrapolation"; }

  std::vector<Proposal> predict(Tick t, const WorldState& ws) override {
    std::vector<Proposal> out;
    for (const Entity& e : ws.entities()) {
      Proposal p;
      p.producer = name();
      p.tick_observed = t;
      p.target = e.id;
      Transform tr;
      tr.position = Vec3{e.transform.position.x + e.velocity.x,
                         e.transform.position.y + e.velocity.y,
                         e.transform.position.z + e.velocity.z};
      p.fields.transform = tr;
      p.confidence = 0.5f;
      out.push_back(std::move(p));
    }
    return out;
  }
};

OW_REGISTER_WORLD_MODEL("linear_extrapolation", LinearExtrapolation)

}  // namespace ow
