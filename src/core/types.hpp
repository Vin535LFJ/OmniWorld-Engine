#pragma once
// Core value types (pure data). Contract: docs/design/00-总体设计.md sec 2.
#include <cstdint>
#include <string>

namespace ow {

using Tick = std::uint64_t;      // monotonic tick domain; tick is the version
using EntityId = std::uint64_t;  // minted by the runtime; 0 = invalid; never reused

struct Vec3 { float x = 0.0f, y = 0.0f, z = 0.0f; };
struct Quat { float w = 1.0f, x = 0.0f, y = 0.0f, z = 0.0f; };
struct Transform { Vec3 position; Quat orientation; };

// Three-state visibility (contract 8): seen / unseen / uncertain.
// Unseen is NOT empty and NOT zero — it means "not observed".
enum class Visibility : std::uint8_t { Seen, Unseen, Uncertain };

struct ClassTag { std::string value; };

}  // namespace ow
