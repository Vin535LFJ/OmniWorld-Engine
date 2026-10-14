#include "observation/observation.hpp"

#include "core/hash.hpp"

#include <cmath>
#include <stdexcept>

namespace ow {
namespace {

struct Color {
  std::uint8_t r, g, b;
};
constexpr Color kBackground{0x10, 0x14, 0x18};

// Palette: FNV-1a of the class tag -> deterministic color. Same tag, same
// color, every run, every machine (reused core/hash.hpp; L2 §2).
Color palette(const std::string& tag) {
  Hasher h;
  h.bytes(tag.data(), tag.size());
  const std::uint64_t v = h.result();
  return Color{static_cast<std::uint8_t>(64 + (v & 0x7F)),
               static_cast<std::uint8_t>(64 + ((v >> 8) & 0x7F)),
               static_cast<std::uint8_t>(64 + ((v >> 16) & 0x7F))};
}

// World window [0,1] x [0,1] on the x/z plane (y is up). The caller's pose is
// recorded in the frame metadata but v1 does not move the projection.
// Out-of-window world coordinates clamp to the border column/row.
int world_to_pixel(float w, int extent) {
  int p = static_cast<int>(std::floor(w * static_cast<float>(extent)));
  if (p < 0) p = 0;
  if (p > extent - 1) p = extent - 1;
  return p;
}

}  // namespace

Observation ObservationGenerator::render(const WorldState& ws, Tick tick,
                                         const RenderConfig& cfg,
                                         Transform pose) {
  if (cfg.width <= 0 || cfg.height <= 0)
    throw std::invalid_argument("render: non-positive resolution");
  if (cfg.footprint <= 0.0f)
    throw std::invalid_argument("render: non-positive footprint");

  Observation out;
  out.tick = tick;
  out.view_pose = pose;
  out.meta.width = cfg.width;
  out.meta.height = cfg.height;

  const std::size_t npx =
      static_cast<std::size_t>(cfg.width) * static_cast<std::size_t>(cfg.height);
  out.rgb.resize(npx * 3);
  for (std::size_t i = 0; i < npx; ++i) {
    out.rgb[i * 3 + 0] = kBackground.r;
    out.rgb[i * 3 + 1] = kBackground.g;
    out.rgb[i * 3 + 2] = kBackground.b;
  }

  for (const Entity& e : ws.entities()) {  // id order = deterministic surface
    const float half = cfg.footprint * 0.5f;
    const float x0 = e.transform.position.x - half;
    const float x1 = e.transform.position.x + half;
    const float z0 = e.transform.position.z - half;
    const float z1 = e.transform.position.z + half;

    const bool fully_in =
        x0 >= 0.0f && z0 >= 0.0f && x1 <= 1.0f && z1 <= 1.0f;
    const bool fully_out =
        x1 <= 0.0f || z1 <= 0.0f || x0 >= 1.0f || z0 >= 1.0f;

    if (fully_out) {
      out.visibility.emplace_back(e.id, Visibility::Unseen);
      continue;
    }
    out.visibility.emplace_back(e.id,
                                fully_in ? Visibility::Seen
                                         : Visibility::Uncertain);

    const int px0 = world_to_pixel(x0, cfg.width);
    const int px1 = world_to_pixel(x1, cfg.width);
    const int py0 = world_to_pixel(z0, cfg.height);
    const int py1 = world_to_pixel(z1, cfg.height);
    const Color c = palette(e.class_tag.value);
    for (int py = py0; py <= py1; ++py) {
      for (int px = px0; px <= px1; ++px) {
        const std::size_t idx =
            (static_cast<std::size_t>(py) *
                 static_cast<std::size_t>(cfg.width) +
             static_cast<std::size_t>(px)) * 3;
        out.rgb[idx + 0] = c.r;
        out.rgb[idx + 1] = c.g;
        out.rgb[idx + 2] = c.b;
      }
    }
  }
  return out;
}

}  // namespace ow
