#pragma once
// Deterministic PRNG (splitmix64). Same seed -> same sequence everywhere.
#include <cstdint>

namespace ow {

class Rng {
 public:
  explicit Rng(std::uint64_t seed) : state_(seed) {}

  std::uint64_t next() {
    state_ += 0x9e3779b97f4a7c15ULL;
    std::uint64_t z = state_;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
  }

  // Uniform [0, 1): top 24 bits scaled by 2^-24 — exactly representable in float.
  float next_unit() {
    return static_cast<float>(next() >> 40) * (1.0f / 16777216.0f);
  }

 private:
  std::uint64_t state_;
};

}  // namespace ow
