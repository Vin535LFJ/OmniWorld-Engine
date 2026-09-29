#include "observation/raster.hpp"

#include "perception/world_palette.hpp"

namespace ow {

Vec2 TopDownCamera::world_from_pixel(float px, float py) const {
  return Vec2{px / 224.0f, py / 224.0f};
}

Vec2 TopDownCamera::pixel_from_world(Vec2 w) const {
  return Vec2{w.x * 224.0f, w.y * 224.0f};
}

void render(const std::vector<TruthBody>& bodies, Image& out, Tick tick) {
  const int W = out.width;
  const int H = out.height;
  if (static_cast<int>(out.rgb.size()) != W * H * 3) {
    out.rgb.assign(static_cast<std::size_t>(W) * H * 3, 0);
  }
  if (static_cast<int>(out.ids.size()) != W * H) {
    out.ids.assign(static_cast<std::size_t>(W) * H, 0);
  }
  out.tick = tick;

  // background
  for (int i = 0; i < W * H; ++i) {
    const std::size_t idx = static_cast<std::size_t>(i);
    out.rgb[idx * 3 + 0] = world::kBackground.r;
    out.rgb[idx * 3 + 1] = world::kBackground.g;
    out.rgb[idx * 3 + 2] = world::kBackground.b;
    out.ids[idx] = 0;
  }

  // draw in id order for deterministic occlusion (bodies are stored in
  // mint order, which is id order)
  for (const TruthBody& b : bodies) {
    const Rgb8 col = world::kPalette[static_cast<std::size_t>(b.class_index)];
    const float r_px = b.radius * static_cast<float>(W);
    const Vec2 c = Vec2{b.pos.x * W, b.pos.y * H};
    const int x0 = static_cast<int>(c.x - r_px);
    const int x1 = static_cast<int>(c.x + r_px + 1.0f);
    const int y0 = static_cast<int>(c.y - r_px);
    const int y1 = static_cast<int>(c.y + r_px + 1.0f);
    for (int py = y0; py <= y1; ++py) {
      if (py < 0 || py >= H) continue;
      for (int px = x0; px <= x1; ++px) {
        if (px < 0 || px >= W) continue;
        const float dx = (static_cast<float>(px) + 0.5f) - c.x;
        const float dy = (static_cast<float>(py) + 0.5f) - c.y;
        if (dx * dx + dy * dy <= r_px * r_px) {
          const std::size_t idx =
              static_cast<std::size_t>(py) * W + static_cast<std::size_t>(px);
          out.rgb[idx * 3 + 0] = col.r;
          out.rgb[idx * 3 + 1] = col.g;
          out.rgb[idx * 3 + 2] = col.b;
          out.ids[idx] = b.id;
        }
      }
    }
  }
}

}  // namespace ow
