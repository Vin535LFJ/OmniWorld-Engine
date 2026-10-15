#pragma once
// Shared internals for the built-in blob detectors (component extraction +
// proposal assembly). This header belongs to the perception module itself —
// the core never sees it; each adapter keeps its single-file shape while
// sharing deterministic detection math.
#include "observation/observation.hpp"
#include "perception/adapter.hpp"

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace ow {

struct BlobComponent {
  float sx_px = 0.0f;
  float sz_px = 0.0f;
  std::uint32_t n = 0;
};

inline bool blob_is_background(const Observation& o, int px, int py) {
  const std::size_t idx =
      (static_cast<std::size_t>(py) * o.meta.width + px) * 3;
  return o.rgb[idx] == kBackgroundRgb[0] && o.rgb[idx + 1] == kBackgroundRgb[1] &&
         o.rgb[idx + 2] == kBackgroundRgb[2];
}

// 4-connected components over non-background pixels. Row-major scan + fixed
// neighbour order: deterministic by construction.
inline std::vector<BlobComponent> blob_components(const Observation& o) {
  const int w = o.meta.width;
  const int h = o.meta.height;
  std::vector<std::uint8_t> seen(static_cast<std::size_t>(w) * h, 0);
  std::vector<BlobComponent> out;
  std::vector<std::pair<int, int>> stack;
  static constexpr int kDirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  for (int py = 0; py < h; ++py) {
    for (int px = 0; px < w; ++px) {
      const std::size_t s = static_cast<std::size_t>(py) * w + px;
      if (seen[s] || blob_is_background(o, px, py)) continue;
      BlobComponent c;
      stack.clear();
      seen[s] = 1;
      stack.emplace_back(px, py);
      while (!stack.empty()) {
        const auto [x, y] = stack.back();
        stack.pop_back();
        ++c.n;
        c.sx_px += static_cast<float>(x);
        c.sz_px += static_cast<float>(y);
        for (const auto& d : kDirs) {
          const int nx = x + d[0];
          const int ny = y + d[1];
          if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
          const std::size_t si = static_cast<std::size_t>(ny) * w + nx;
          if (seen[si] || blob_is_background(o, nx, ny)) continue;
          seen[si] = 1;
          stack.emplace_back(nx, ny);
        }
      }
      out.push_back(c);
    }
  }
  return out;
}

inline Proposal make_blob_proposal(const char* producer, const Observation& obs,
                                   const BlobComponent& c, float confidence) {
  Proposal p;
  p.producer = producer;
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
  p.confidence = confidence;
  return p;
}

}  // namespace ow
