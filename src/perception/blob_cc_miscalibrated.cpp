#include "perception/adapter.hpp"

#include <map>
#include <memory>

// Deliberately miscalibrated detector — Gate 1c's test target. Same
// detections as blob_cc (delegates to it), but confidence is INVERTED:
// it is confidently wrong. The coherence metrics must flag this model
// purely from its (confidence x correctness) joint behavior.
namespace ow {
namespace {

class MiscalibratedDetector final : public PerceptionAdapter {
 public:
  std::string name() const override { return "blob_cc_miscalibrated"; }

  std::vector<Proposal> detect(const Image& img, const TopDownCamera& cam,
                               Tick observed) const override {
    auto base = create_perception("blob_cc");
    auto props = base ? base->detect(img, cam, observed)
                      : std::vector<Proposal>{};
    for (Proposal& p : props) {
      p.producer = name();
      p.confidence = 1.0f - p.confidence;  // inverted calibration
    }
    return props;
  }
};

}  // namespace

OW_REGISTER_PERCEPTION("blob_cc_miscalibrated", [] {
  return std::unique_ptr<PerceptionAdapter>(new MiscalibratedDetector());
});

}  // namespace ow
