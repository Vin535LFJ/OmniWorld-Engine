#include "perception/blob_internals.hpp"

namespace ow {

// blob_detect_overconfident: same detector, but claims 0.99 confidence for
// everything regardless of evidence — the deliberate miscalibration target
// for ECE discrimination (Gate 1c analog / contract 7). Kept as its own
// single file so "a second adapter = one new file, zero core edits".
class BlobDetectOverconfident final : public PerceptionAdapter {
 public:
  const char* name() const override { return "blob_detect_overconfident"; }

  std::vector<Proposal> observe(const Observation& obs) override {
    std::vector<Proposal> out;
    for (const BlobComponent& c : blob_components(obs)) {
      Proposal p;
      p.producer = name();
      p.tick_observed = obs.tick;
      p.target = NewEntity{};
      Transform t;
      t.position = Vec3{
          (c.sx_px / static_cast<float>(c.n) + 0.5f) /
                  static_cast<float>(obs.meta.width),
          0.0f,
          (c.sz_px / static_cast<float>(c.n) + 0.5f) /
              static_cast<float>(obs.meta.height)};
      p.fields.transform = t;
      p.confidence = 0.99f;  // the calibration fault under test
      out.push_back(std::move(p));
    }
    return out;
  }
};

OW_REGISTER_PERCEPTION("blob_detect_overconfident", BlobDetectOverconfident)

}  // namespace ow
