#pragma once
// Tiny deterministic MLP: 8 -> 24 -> 24 -> 2 (input: one-hot class + pos +
// vel; output: predicted next pos on the plane). Pure C++ SGD — no Python,
// no frameworks, bit-exact reproducible for a fixed seed. NOT a foundation
// model: ~3k parameters trained in-process on the recorded rollout.
#include "core/rng.hpp"
#include "core/types.hpp"

#include <cstdint>
#include <vector>

namespace ow {

struct MlpModel {
  static constexpr int kIn = 8;   // onehot(4) + pos.x + pos.z + vel.x + vel.z
  static constexpr int kH1 = 24;
  static constexpr int kH2 = 24;
  static constexpr int kOut = 2;

  // plain row-major weight matrices
  std::vector<float> w1, b1, w2, b2, w3, b3;

  void init(std::uint64_t seed);
  void forward(const float in[kIn], float out[kOut]) const;
};

// Builds the feature vector for one entity.
void mlp_input(int class_index, Vec2 pos, Vec2 vel, float out[MlpModel::kIn]);

// Deterministic training: sample-by-sample SGD, MSE, fixed order.
// samples: flat array of (kIn inputs, kOut targets) pairs.
// Returns the final training loss.
float mlp_train(MlpModel& m, const std::vector<float>& inputs,
               const std::vector<float>& targets, int n_samples, int epochs,
               float lr, std::uint64_t seed);

// Digest over all weights (determinism test hook).
std::uint64_t mlp_weights_digest(const MlpModel& m);

}  // namespace ow
