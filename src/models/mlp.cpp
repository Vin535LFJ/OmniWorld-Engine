#include "models/mlp.hpp"

#include "core/hash.hpp"

#include <cmath>
#include <cstring>

namespace ow {
namespace {
inline float rand_uniform(Pcg32& rng, float scale) {
  return (rng.uniform() * 2.0f - 1.0f) * scale;
}
}  // namespace

void MlpModel::init(std::uint64_t seed) {
  Pcg32 rng = Pcg32::seeded(seed, 0x5EEDu);
  w1.assign(static_cast<std::size_t>(kH1) * kIn, 0.0f);
  b1.assign(kH1, 0.0f);
  w2.assign(static_cast<std::size_t>(kH2) * kH1, 0.0f);
  b2.assign(kH2, 0.0f);
  w3.assign(static_cast<std::size_t>(kOut) * kH2, 0.0f);
  b3.assign(kOut, 0.0f);
  const float s1 = 1.0f / std::sqrt(static_cast<float>(kIn));
  const float s2 = 1.0f / std::sqrt(static_cast<float>(kH1));
  const float s3 = 1.0f / std::sqrt(static_cast<float>(kH2));
  for (float& v : w1) v = rand_uniform(rng, s1);
  for (float& v : w2) v = rand_uniform(rng, s2);
  for (float& v : w3) v = rand_uniform(rng, s3);
}

void MlpModel::forward(const float in[kIn], float out[kOut]) const {
  float h1[kH1], h2[kH2];
  for (int i = 0; i < kH1; ++i) {
    float s = b1[static_cast<std::size_t>(i)];
    const float* row = &w1[static_cast<std::size_t>(i) * kIn];
    for (int j = 0; j < kIn; ++j) s += row[j] * in[j];
    h1[i] = s > 0.0f ? s : 0.0f;  // ReLU
  }
  for (int i = 0; i < kH2; ++i) {
    float s = b2[static_cast<std::size_t>(i)];
    const float* row = &w2[static_cast<std::size_t>(i) * kH1];
    for (int j = 0; j < kH1; ++j) s += row[j] * h1[j];
    h2[i] = s > 0.0f ? s : 0.0f;
  }
  for (int i = 0; i < kOut; ++i) {
    float s = b3[static_cast<std::size_t>(i)];
    const float* row = &w3[static_cast<std::size_t>(i) * kH2];
    for (int j = 0; j < kH2; ++j) s += row[j] * h2[j];
    out[i] = s;
  }
}

void mlp_input(int class_index, Vec2 pos, Vec2 vel, float out[MlpModel::kIn]) {
  for (int i = 0; i < MlpModel::kIn; ++i) out[i] = 0.0f;
  out[class_index & 3] = 1.0f;
  out[4] = pos.x;
  out[5] = pos.y;
  out[6] = vel.x;
  out[7] = vel.y;
}

float mlp_train(MlpModel& m, const std::vector<float>& inputs,
                const std::vector<float>& targets, int n_samples, int epochs,
                float lr, std::uint64_t seed) {
  m.init(seed);
  float last_loss = 0.0f;
  std::vector<float> in(MlpModel::kIn), tgt(MlpModel::kOut);
  for (int e = 0; e < epochs; ++e) {
    double loss_sum = 0.0;
    for (int s = 0; s < n_samples; ++s) {
      const float* xi = &inputs[static_cast<std::size_t>(s) * MlpModel::kIn];
      const float* xt = &targets[static_cast<std::size_t>(s) * MlpModel::kOut];
      for (int j = 0; j < MlpModel::kIn; ++j) in[static_cast<std::size_t>(j)] = xi[j];
      for (int j = 0; j < MlpModel::kOut; ++j) tgt[static_cast<std::size_t>(j)] = xt[j];

      // forward
      float h1[MlpModel::kH1], h2[MlpModel::kH2], out[MlpModel::kOut];
      for (int i = 0; i < MlpModel::kH1; ++i) {
        float a = m.b1[static_cast<std::size_t>(i)];
        const float* row = &m.w1[static_cast<std::size_t>(i) * MlpModel::kIn];
        for (int j = 0; j < MlpModel::kIn; ++j) a += row[j] * in[static_cast<std::size_t>(j)];
        h1[i] = a;
      }
      float r1[MlpModel::kH1];
      for (int i = 0; i < MlpModel::kH1; ++i) r1[i] = h1[i] > 0.0f ? h1[i] : 0.0f;
      for (int i = 0; i < MlpModel::kH2; ++i) {
        float a = m.b2[static_cast<std::size_t>(i)];
        const float* row = &m.w2[static_cast<std::size_t>(i) * MlpModel::kH1];
        for (int j = 0; j < MlpModel::kH1; ++j) a += row[j] * r1[j];
        h2[i] = a;
      }
      float r2[MlpModel::kH2];
      for (int i = 0; i < MlpModel::kH2; ++i) r2[i] = h2[i] > 0.0f ? h2[i] : 0.0f;
      for (int i = 0; i < MlpModel::kOut; ++i) {
        float a = m.b3[static_cast<std::size_t>(i)];
        const float* row = &m.w3[static_cast<std::size_t>(i) * MlpModel::kH2];
        for (int j = 0; j < MlpModel::kH2; ++j) a += row[j] * r2[j];
        out[i] = a;
      }

      // loss + output delta
      float d3[MlpModel::kOut];
      float loss = 0.0f;
      for (int i = 0; i < MlpModel::kOut; ++i) {
        const float err = out[i] - tgt[static_cast<std::size_t>(i)];
        d3[i] = err;  // MSE: dL/dout = err
        loss += err * err;
      }
      loss_sum += loss;

      // backward
      float d2[MlpModel::kH2];
      for (int i = 0; i < MlpModel::kH2; ++i) {
        float a = 0.0f;
        for (int k = 0; k < MlpModel::kOut; ++k) {
          a += m.w3[static_cast<std::size_t>(k) * MlpModel::kH2 +
                    static_cast<std::size_t>(i)] * d3[k];
        }
        d2[i] = (h2[i] > 0.0f) ? a : 0.0f;
      }
      float d1[MlpModel::kH1];
      for (int i = 0; i < MlpModel::kH1; ++i) {
        float a = 0.0f;
        for (int k = 0; k < MlpModel::kH2; ++k) {
          a += m.w2[static_cast<std::size_t>(k) * MlpModel::kH1 +
                    static_cast<std::size_t>(i)] * d2[k];
        }
        d1[i] = (h1[i] > 0.0f) ? a : 0.0f;
      }

      // SGD update
      for (int i = 0; i < MlpModel::kOut; ++i) {
        float* row = &m.w3[static_cast<std::size_t>(i) * MlpModel::kH2];
        for (int j = 0; j < MlpModel::kH2; ++j) row[j] -= lr * d3[i] * r2[j];
        m.b3[static_cast<std::size_t>(i)] -= lr * d3[i];
      }
      for (int i = 0; i < MlpModel::kH2; ++i) {
        float* row = &m.w2[static_cast<std::size_t>(i) * MlpModel::kH1];
        for (int j = 0; j < MlpModel::kH1; ++j) row[j] -= lr * d2[i] * r1[j];
        m.b2[static_cast<std::size_t>(i)] -= lr * d2[i];
      }
      for (int i = 0; i < MlpModel::kH1; ++i) {
        float* row = &m.w1[static_cast<std::size_t>(i) * MlpModel::kIn];
        for (int j = 0; j < MlpModel::kIn; ++j) {
          row[j] -= lr * d1[i] * in[static_cast<std::size_t>(j)];
        }
        m.b1[static_cast<std::size_t>(i)] -= lr * d1[i];
      }
    }
    last_loss = static_cast<float>(loss_sum / n_samples);
  }
  return last_loss;
}

std::uint64_t mlp_weights_digest(const MlpModel& m) {
  Hasher h;
  h.label("MlpWeights");
  auto addv = [&h](const std::vector<float>& v) {
    h.add(v.data(), v.size() * sizeof(float));
  };
  addv(m.w1);
  addv(m.b1);
  addv(m.w2);
  addv(m.b2);
  addv(m.w3);
  addv(m.b3);
  return h.h;
}

}  // namespace ow
