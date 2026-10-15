#include "perception/blob_internals.hpp"

namespace ow {

// blob_detect: honest-confidence detector. Sees only the Observation, emits
// only Proposals (no class guesses; no sight of WorldState).
class BlobDetect final : public PerceptionAdapter {
 public:
  const char* name() const override { return "blob_detect"; }

  std::vector<Proposal> observe(const Observation& obs) override {
    std::vector<Proposal> out;
    for (const BlobComponent& c : blob_components(obs)) {
      // honest: scaled by blob area, capped below certainty
      const float conf = 0.70f + 0.02f * static_cast<float>(c.n);
      out.push_back(make_blob_proposal(name(), obs, c, conf > 0.95f ? 0.95f : conf));
    }
    return out;
  }
};

OW_REGISTER_PERCEPTION("blob_detect", BlobDetect)

}  // namespace ow
