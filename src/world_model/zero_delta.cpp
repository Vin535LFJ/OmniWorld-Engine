#include "world_model/world_model.hpp"

namespace ow {

// zero_delta: "everything stays where it is" — the floor model for
// divergence comparison. Fixed 0.5 confidence (v0.1 as-built fact).
class ZeroDelta final : public WorldModelAdapter {
 public:
  const char* name() const override { return "zero_delta"; }

  std::vector<Proposal> predict(Tick t, const WorldState& ws) override {
    std::vector<Proposal> out;
    for (const Entity& e : ws.entities()) {
      Proposal p;
      p.producer = name();
      p.tick_observed = t;
      p.target = e.id;  // explicit: predicting THIS entity
      Transform tr;
      tr.position = e.transform.position;
      p.fields.transform = tr;
      p.confidence = 0.5f;
      out.push_back(std::move(p));
    }
    return out;
  }
};

OW_REGISTER_WORLD_MODEL("zero_delta", ZeroDelta)

}  // namespace ow
