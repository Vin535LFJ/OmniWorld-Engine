#pragma once
// Stable FNV-1a hash over raw bytes — used by determinism tests and, later,
// by regression tooling.
#include <cstddef>
#include <cstdint>

namespace ow {

class Hasher {
 public:
  void bytes(const void* data, std::size_t n) {
    const auto* p = static_cast<const unsigned char*>(data);
    for (std::size_t i = 0; i < n; ++i) {
      h_ ^= p[i];
      h_ *= 0x100000001b3ULL;
    }
  }

  template <typename T>
  void value(const T& v) {
    bytes(&v, sizeof(T));
  }

  std::uint64_t result() const { return h_; }

 private:
  std::uint64_t h_ = 0xcbf29ce484222325ULL;
};

}  // namespace ow
