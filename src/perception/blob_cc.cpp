#include "perception/adapter.hpp"

#include "perception/world_palette.hpp"

// Detector A — "blob_cc": per-class color threshold + 4-connected
// components + sub-pixel centroid. A classical CV detector; its "model
// knowledge" is the world palette (legitimate training prior).
// Confidence: strong-match purity + size prior.
namespace ow {
namespace {

struct Blob {
  float cx = 0, cy = 0;    // pixel-space centroid
  int area = 0;
  int strong = 0;          // pixels within half tolerance
};

class BlobCcDetector final : public PerceptionAdapter {
 public:
  std::string name() const override { return "blob_cc"; }

  std::vector<Proposal> detect(const Image& img, const TopDownCamera& cam,
                               Tick observed) const override {
    const int W = img.width, H = img.height;
    std::vector<Proposal> proposals;
    std::vector<std::uint8_t> mask(static_cast<std::size_t>(W) * H, 0);

    for (int c = 0; c < world::kClasses; ++c) {
      const Rgb8 target = world::kPalette[static_cast<std::size_t>(c)];
      std::fill(mask.begin(), mask.end(), std::uint8_t{0});
      for (int i = 0; i < W * H; ++i) {
        const std::uint8_t* p = &img.rgb[static_cast<std::size_t>(i) * 3];
        const int d = std::abs(int(p[0]) - int(target.r)) +
                      std::abs(int(p[1]) - int(target.g)) +
                      std::abs(int(p[2]) - int(target.b));
        mask[static_cast<std::size_t>(i)] = d <= 72 ? 2 : 0;
      }

      // connected components (4-conn, iterative stack)
      std::vector<int> stack;
      for (int start = 0; start < W * H; ++start) {
        if (mask[static_cast<std::size_t>(start)] == 0) continue;
        Blob blob;
        stack.clear();
        stack.push_back(start);
        mask[static_cast<std::size_t>(start)] = 1;  // visited
        double sx = 0, sy = 0;
        while (!stack.empty()) {
          const int idx = stack.back();
          stack.pop_back();
          const int px = idx % W, py = idx / W;
          sx += px + 0.5;
          sy += py + 0.5;
          ++blob.area;
          const std::uint8_t* q = &img.rgb[static_cast<std::size_t>(idx) * 3];
          const int d = std::abs(int(q[0]) - int(target.r)) +
                        std::abs(int(q[1]) - int(target.g)) +
                        std::abs(int(q[2]) - int(target.b));
          if (d <= 36) ++blob.strong;
          const int nb[4] = {idx - 1, idx + 1, idx - W, idx + W};
          const bool valid[4] = {px > 0, px < W - 1, py > 0, py < H - 1};
          for (int k = 0; k < 4; ++k) {
            if (!valid[k]) continue;
            if (mask[static_cast<std::size_t>(nb[k])] == 2) {
              mask[static_cast<std::size_t>(nb[k])] = 1;
              stack.push_back(nb[k]);
            }
          }
        }
        blob.cx = static_cast<float>(sx / blob.area);
        blob.cy = static_cast<float>(sy / blob.area);
        if (blob.area < 10) continue;

        Proposal prop;
        prop.producer = name();
        prop.tick_observed = observed;
        prop.class_tag = world::kNames[static_cast<std::size_t>(c)];
        const Vec2 w = cam.world_from_pixel(blob.cx, blob.cy);
        prop.position = Vec3{w.x, 0.0f, w.y};
        const float purity =
            static_cast<float>(blob.strong) / static_cast<float>(blob.area);
        const float size_prior =
            clampf(static_cast<float>(blob.area) / 120.0f, 0.0f, 1.0f);
        prop.confidence = clampf(0.35f + 0.45f * purity + 0.20f * size_prior,
                                 0.0f, 0.99f);
        proposals.push_back(std::move(prop));
      }
    }
    return proposals;
  }
};

}  // namespace

OW_REGISTER_PERCEPTION(
    "blob_cc", [] { return std::unique_ptr<PerceptionAdapter>(new BlobCcDetector()); });

}  // namespace ow
