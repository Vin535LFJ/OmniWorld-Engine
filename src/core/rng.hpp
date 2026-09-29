#pragma once
// PCG32 — small, well-defined, deterministic RNG. Same seed + same binary
// => identical sequence. Used by the truth world and the MLP trainer.
#include <cstdint>

namespace ow {

struct Pcg32 {
  std::uint64_t state = 0;
  std::uint64_t inc = 1;

  static Pcg32 seeded(std::uint64_t seed, std::uint64_t seq) {
    Pcg32 r;
    r.state = 0u;
    r.inc = (seq << 1u) | 1u;
    r.next();
    r.state += seed;
    r.next();
    return r;
  }

  std::uint32_t next() {
    std::uint64_t old = state;
    state = old * 6364136223846793005ULL + inc;
    std::uint32_t xorshifted =
        static_cast<std::uint32_t>(((old >> 18u) ^ old) >> 27u);
    std::uint32_t rot = static_cast<std::uint32_t>(old >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31u));
  }

  // uniform in [0, 1)
  float uniform() {
    return static_cast<float>(next() >> 8) * (1.0f / 16777216.0f);
  }

  // uniform in [a, b)
  float uniform_range(float a, float b) { return a + (b - a) * uniform(); }
};

}  // namespace ow
