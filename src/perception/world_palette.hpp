#pragma once
// The v0 synthetic world's class vocabulary: colors and names shared by the
// truth world (spawn) and the perception adapters (detection prior). This
// is the world's "training data" — adapters are allowed to know it.
#include "core/types.hpp"

#include <array>
#include <string>

namespace ow::world {

inline constexpr int kClasses = 4;
inline const std::array<Rgb8, kClasses> kPalette = {{
    {230, 60, 60},   // red
    {60, 120, 235},  // blue
    {70, 200, 90},   // green
    {235, 200, 60},  // yellow
}};
inline const std::array<std::string, kClasses> kNames = {{
    "red", "blue", "green", "yellow",
}};
inline constexpr Rgb8 kBackground{28, 28, 34};

}  // namespace ow::world
