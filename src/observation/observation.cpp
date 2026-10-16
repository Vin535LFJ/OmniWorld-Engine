#include "observation/observation.hpp"

#include "core/hash.hpp"

#include <cmath>
#include <stdexcept>

namespace ow {
namespace {

struct Color {
  std::uint8_t r, g, b;
};

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

// World window [0,1] x [0,1] on the x/z plane (y is up). Projection lives in
// ViewCamera now (merged camera, stage C slice C2); the free helper is gone.

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
  out.view.pose = pose;  // merged camera: single construction point
  out.view.width = cfg.width;
  out.view.height = cfg.height;
  out.meta.width = out.view.width;   // dims cannot disagree with the camera
  out.meta.height = out.view.height;

  const std::size_t npx =
      static_cast<std::size_t>(cfg.width) * static_cast<std::size_t>(cfg.height);
  out.rgb.resize(npx * 3);
  for (std::size_t i = 0; i < npx; ++i) {
    out.rgb[i * 3 + 0] = kBackgroundRgb[0];
    out.rgb[i * 3 + 1] = kBackgroundRgb[1];
    out.rgb[i * 3 + 2] = kBackgroundRgb[2];
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

    const int px0 = out.view.pixel_x(x0);
    const int px1 = out.view.pixel_x(x1);
    const int py0 = out.view.pixel_z(z0);
    const int py1 = out.view.pixel_z(z1);
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
