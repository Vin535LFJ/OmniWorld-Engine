#pragma once
// Observation generation (contract 8): bounded view of the world state =
// RGB frame + frame metadata + per-entity three-state visibility.
// Resolution is a CALLER-side parameter (RenderConfig); no default-resolution
// constant may exist inside this module (the legacy-224 bug class is removed
// by structure, not by convention).
#include "core/types.hpp"
#include "world_state/world_state.hpp"

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

// Visibility is explicitly paired with entity ids, in id order — consumers
// never couple to entity iteration order (L2 deviation from L1 sketch).
struct Observation {
  Tick tick = 0;
  Transform view_pose;
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
