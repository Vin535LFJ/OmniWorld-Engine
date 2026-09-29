#pragma once
// OmniWorld core scalar/vector types. v0 world frame: X/Z is the ground
// plane [0,1]^2, +Y is up. All bodies live on y=0 (orthographic top-down
// sensor). Float only; determinism is same-binary (MSVC SSE2, /fp:precise).
#include <cmath>
#include <cstdint>

namespace ow {

using EntityId = std::uint64_t;  // runtime-minted, never reused
using Tick = std::uint64_t;      // monotonic tick domain, runtime-owned
using ComponentId = std::uint32_t;

struct Vec2 {
  float x = 0.0f, y = 0.0f;
};
inline Vec2 operator+(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
inline Vec2 operator-(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }
inline Vec2 operator*(Vec2 a, float s) { return {a.x * s, a.y * s}; }
inline float dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
inline float length(Vec2 a) { return std::sqrt(dot(a, a)); }

struct Vec3 {
  float x = 0.0f, y = 0.0f, z = 0.0f;
};
inline Vec3 operator+(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline Vec3 operator-(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline Vec3 operator*(Vec3 a, float s) { return {a.x * s, a.y * s, a.z * s}; }
inline float dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float length(Vec3 a) { return std::sqrt(dot(a, a)); }
inline float distance(Vec3 a, Vec3 b) { return length(a - b); }

// v0 keeps the contract's pos+quat shape; nothing rotates in Grounding
// Loop v0, so rotation stays identity everywhere.
struct Quat {
  float w = 1.0f, x = 0.0f, y = 0.0f, z = 0.0f;
};

struct Rgb8 {
  std::uint8_t r = 0, g = 0, b = 0;
};

inline float clampf(float v, float lo, float hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}

}  // namespace ow
