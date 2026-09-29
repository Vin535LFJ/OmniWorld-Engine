#pragma once
// FNV-1a 64-bit streaming hasher. Determinism gate: state and observation
// sequences are hashed and compared across runs (same binary => bit-exact).
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

namespace ow {

struct Hasher {
  std::uint64_t h = 14695981039346656037ULL;  // FNV offset basis

  void add(const void* data, std::size_t n) {
    const unsigned char* p = static_cast<const unsigned char*>(data);
    for (std::size_t i = 0; i < n; ++i) {
      h ^= p[i];
      h *= 1099511628211ULL;  // FNV prime
    }
  }
  void add_u64(std::uint64_t v) { add(&v, sizeof(v)); }
  void add_u32(std::uint32_t v) { add(&v, sizeof(v)); }
  void add_f32(float v) { add(&v, sizeof(v)); }
  void add_str(const std::string& s) {
    add_u64(s.size());
    add(s.data(), s.size());
  }
  // label helper: mix in a tag so fields can't be transposed silently
  void label(const char* tag) {
    std::size_t n = std::strlen(tag);
    add_u32(static_cast<std::uint32_t>(n));
    add(tag, n);
  }
};

}  // namespace ow
