#include "perception/adapter.hpp"

#include "perception/world_palette.hpp"

#include <array>

// Detector B — "blob_grid": coarse 8x8 block voting + weighted-centroid
// refinement. A genuinely different pipeline from blob_cc (block-vote
// coarse localization + soft-mask weighted centroid instead of connected
// components), producing a different failure profile — exactly what
// Gate 1b/c needs: two models, one contract.
namespace ow {
namespace {

constexpr int kBlocks = 8;
constexpr int kMinBlockVotes = 25;   // matched pixels required to label a block
constexpr float kMatchDist = 72.0f;  // L1 color distance for a pixel match

class BlobGridDetector final : public PerceptionAdapter {
 public:
  std::string name() const override { return "blob_grid"; }

  std::vector<Proposal> detect(const Image& img, const TopDownCamera& cam,
                               Tick observed) const override {
    const int W = img.width, H = img.height;
    const int bw = W / kBlocks, bh = H / kBlocks;

    // 1) per-block class votes (pixel-level match, block-level decision)
    std::array<int, kBlocks * kBlocks> label{};
    label.fill(-1);
    for (int by = 0; by < kBlocks; ++by) {
      for (int bx = 0; bx < kBlocks; ++bx) {
        int votes[world::kClasses] = {0, 0, 0, 0};
        for (int py = by * bh; py < (by + 1) * bh; ++py) {
          for (int px = bx * bw; px < (bx + 1) * bw; ++px) {
            const std::size_t i = static_cast<std::size_t>(py) * W + px;
            const float r = float(img.rgb[i * 3 + 0]);
            const float g = float(img.rgb[i * 3 + 1]);
            const float b = float(img.rgb[i * 3 + 2]);
            int best = -1;
            float bestd = kMatchDist;
            for (int c = 0; c < world::kClasses; ++c) {
              const Rgb8 t = world::kPalette[static_cast<std::size_t>(c)];
              const float d = std::fabs(r - t.r) + std::fabs(g - t.g) +
                              std::fabs(b - t.b);
              if (d < bestd) {
                bestd = d;
                best = c;
              }
            }
            if (best >= 0) ++votes[best];
          }
        }
        int best = -1, bestn = kMinBlockVotes;
        for (int c = 0; c < world::kClasses; ++c) {
          if (votes[c] > bestn) {
            bestn = votes[c];
            best = c;
          }
        }
        label[static_cast<std::size_t>(by) * kBlocks + bx] = best;
      }
    }

    // 2) connected block clusters per class
    std::array<std::uint8_t, kBlocks * kBlocks> seen{};
    seen.fill(0);
    std::vector<Proposal> proposals;
    std::vector<int> stack;
    for (int b0 = 0; b0 < kBlocks * kBlocks; ++b0) {
      if (seen[b0] || label[b0] < 0) continue;
      const int cls = label[b0];
      std::vector<int> cluster;
      stack.clear();
      stack.push_back(b0);
      seen[b0] = 1;
      while (!stack.empty()) {
        const int b = stack.back();
        stack.pop_back();
        cluster.push_back(b);
        const int bx = b % kBlocks, by = b / kBlocks;
        const int nb[4] = {b - 1, b + 1, b - kBlocks, b + kBlocks};
        const bool valid[4] = {bx > 0, bx < kBlocks - 1, by > 0,
                               by < kBlocks - 1};
        for (int k = 0; k < 4; ++k) {
          if (!valid[k]) continue;
          if (!seen[nb[k]] && label[nb[k]] == cls) {
            seen[nb[k]] = 1;
            stack.push_back(nb[k]);
          }
        }
      }

      // 3) refine: soft-mask weighted centroid over the cluster's pixels
      const Rgb8 target = world::kPalette[static_cast<std::size_t>(cls)];
      double sw = 0, sx = 0, sy = 0;
      long npix = 0;
      for (int b : cluster) {
        const int bx = b % kBlocks, by = b / kBlocks;
        for (int py = by * bh; py < (by + 1) * bh; ++py) {
          for (int px = bx * bw; px < (bx + 1) * bw; ++px) {
            const std::size_t i = static_cast<std::size_t>(py) * W + px;
            const float d = std::fabs(float(img.rgb[i * 3 + 0]) - target.r) +
                            std::fabs(float(img.rgb[i * 3 + 1]) - target.g) +
                            std::fabs(float(img.rgb[i * 3 + 2]) - target.b);
            const float w = clampf(1.0f - d / kMatchDist, 0.0f, 1.0f);
            sw += w;
            sx += w * (px + 0.5);
            sy += w * (py + 0.5);
            ++npix;
          }
        }
      }
      if (sw <= 0.0) continue;
      const float wmean = static_cast<float>(sw / npix);
      const float coverage =
          clampf(static_cast<float>(cluster.size()) / 6.0f, 0.0f, 1.0f);

      Proposal prop;
      prop.producer = name();
      prop.tick_observed = observed;
      prop.class_tag = world::kNames[static_cast<std::size_t>(cls)];
      const Vec2 wpos = cam.world_from_pixel(
          static_cast<float>(sx / sw), static_cast<float>(sy / sw));
      prop.position = Vec3{wpos.x, 0.0f, wpos.y};
      prop.confidence = clampf(0.20f + 0.60f * wmean + 0.20f * coverage,
                               0.0f, 0.99f);
      proposals.push_back(std::move(prop));
    }
    return proposals;
  }
};

}  // namespace

OW_REGISTER_PERCEPTION(
    "blob_grid", [] { return std::unique_ptr<PerceptionAdapter>(new BlobGridDetector()); });

}  // namespace ow
