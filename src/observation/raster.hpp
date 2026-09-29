#pragma once
// Offscreen observation generator: orthographic top-down "sensor" over the
// [0,1]^2 plane, 224x224 RGB8. Deliberately minimal (PLAN.md: visual
// fidelity is not the claim). Also emits an id map and a depth plane —
// used ONLY by the metrics layer for scoring; adapters see RGB only.
// Frame buffer carries the memory-contract metadata fields (PLAN.md 7).
#include "core/types.hpp"
#include "truth_world/truth_world.hpp"

#include <vector>

namespace ow {

struct Image {
  int width = 224;
  int height = 224;
  std::vector<std::uint8_t> rgb;  // W*H*3, row-major, top-left origin
  std::vector<EntityId> ids;      // W*H, 0 = background (metrics only)
  // memory contract metadata
  const char* backend = "cpu";
  const char* device = "host";
  const char* format = "RGB8";
  const char* lifetime = "frame";
  Tick tick = 0;

  int pitch() const { return width * 3; }
};

// Orthographic top-down camera covering the [0,1]^2 plane exactly.
// view_transform: looking straight down from +Y (identity rotation, -Y axis).
struct TopDownCamera {
  // pixel-space center (px in [0,W)) -> world (x, z)
  Vec2 world_from_pixel(float px, float py) const;
  // world (x, z) -> pixel-space (continuous)
  Vec2 pixel_from_world(Vec2 w) const;
  Vec2 pixel_from_world(float wx, float wz) const {
    return pixel_from_world(Vec2{wx, wz});
  }
};

// Renders bodies in id order (deterministic occlusion: later ids overwrite).
void render(const std::vector<TruthBody>& bodies, Image& out, Tick tick);

}  // namespace ow
