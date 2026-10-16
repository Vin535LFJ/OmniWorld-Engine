#pragma once
// Observation generation (contract 8): bounded view of the world state =
// RGB frame + frame metadata + per-entity three-state visibility.
// Resolution is a CALLER-side parameter (RenderConfig); no default-resolution
// constant may exist inside this module (the legacy-224 bug class is removed
// by structure, not by convention).
#include "core/types.hpp"
#include "world_state/world_state.hpp"

#include <cmath>
#include <string>
#include <utility>
#include <vector>

namespace ow {

// Observation-domain background sentinel ("nothing here" color). Adapters
// consume it; they must not redefine it (slice 03 promoted it to the header).
inline constexpr std::uint8_t kBackgroundRgb[3] = {0x10, 0x14, 0x18};

struct FrameMeta {
  // Reference-mode labels: this generator is the CPU path (facts, not
  // parameters; performance mode labels arrive with stage B/C).
  std::string backend = "cpu";
  std::string device = "host";
  std::string format = "RGB8";
  int width = 0;
  int height = 0;
};

// Camera merged INTO the observation (arch candidate #3, stage C slice C2):
// an observation carries HOW it was seen — the pose and the projection
// between world (x/z plane, y up) and pixels. Resolution is the camera's
// own property, never a module constant (the legacy-224 bug class stays
// structurally removed).
struct ViewCamera {
  Transform pose;  // 契约⑧ the only second coordinate frame allowed
  int width = 0;
  int height = 0;

  // world (x, z) -> pixel column/row; out-of-window clamps to the border.
  int pixel_x(float wx) const {
    int p = static_cast<int>(std::floor(wx * static_cast<float>(width)));
    if (p < 0) p = 0;
    if (p > width - 1) p = width - 1;
    return p;
  }
  int pixel_z(float wz) const {
    int p = static_cast<int>(std::floor(wz * static_cast<float>(height)));
    if (p < 0) p = 0;
    if (p > height - 1) p = height - 1;
    return p;
  }
  // Inverse of the above (pixel indices are pixel-box centers).
  float world_x(int px) const {
    return (static_cast<float>(px) + 0.5f) / static_cast<float>(width);
  }
  float world_z(int py) const {
    return (static_cast<float>(py) + 0.5f) / static_cast<float>(height);
  }
};

// Visibility is explicitly paired with entity ids, in id order — consumers
// never couple to entity iteration order (L2 deviation from L1 sketch).
struct Observation {
  Tick tick = 0;
  ViewCamera view;  // merged camera: pose + resolution are the observation's own
  std::string sensor_kind = "rgb_topdown";
  FrameMeta meta;
  std::vector<std::uint8_t> rgb;  // W*H*3, row-major, top-left origin
  std::vector<std::pair<EntityId, Visibility>> visibility;  // id order
};

// Caller-owned render configuration: resolution + entity footprint.
// NO default member values — defaults belong to the caller's config object.
struct RenderConfig {
  int width;
  int height;
  float footprint;  // entity square size in world units
};

class ObservationGenerator {
 public:
  Observation render(const WorldState&, Tick, const RenderConfig&, Transform view_pose);
};

}  // namespace ow
