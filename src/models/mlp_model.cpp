#include "models/mlp.hpp"
#include "models/rollout.hpp"
#include "models/world_model.hpp"

#include "perception/world_palette.hpp"

// MLP dynamics model — the "tiny self-trained world model" (Phase 1):
// a ~3k-parameter MLP trained in-process (pure C++ SGD) on the recorded
// rollout, predicting each entity's next-tick position. Deterministic for
// a fixed seed. This is a learning-based world model in the loop, NOT a
// foundation model.
namespace ow {
namespace {

class MlpDynamicsModel final : public WorldModelAdapter {
 public:
  std::string name() const override { return "mlp_dynamics"; }

  void fit(const Rollout& rollout) override {
    // build the training set: input at t, target position at t+1 (by id)
    inputs_.clear();
    targets_.clear();
    n_samples_ = 0;
    for (std::size_t t = 0; t + 1 < rollout.ticks.size(); ++t) {
      const RolloutTick& cur = rollout.ticks[t];
      const RolloutTick& next = rollout.ticks[t + 1];
      for (const RolloutEntity& e : cur.entities) {
        const RolloutEntity* rec = nullptr;
        for (const RolloutEntity& cand : next.entities) {
          if (cand.id == e.id) {
            rec = &cand;
            break;
          }
        }
        if (!rec) continue;
        float in[MlpModel::kIn];
        mlp_input(e.class_index, e.pos, e.vel, in);
        for (float v : in) inputs_.push_back(v);
        // predict the DELTA (next - current): a near-linear map from the
        // velocity features — trainable in a few epochs, and it lets the
        // net spend capacity on the nonlinear part (wall bounces).
        targets_.push_back(rec->pos.x - e.pos.x);
        targets_.push_back(rec->pos.y - e.pos.y);
        ++n_samples_;
      }
    }
    // fixed hyperparameters => deterministic training
    train_loss_ =
        mlp_train(model_, inputs_, targets_, n_samples_, kEpochs, kLr, kTrainSeed);
  }

  std::vector<Proposal> predict(
      Tick current, const std::vector<ModelInputEntity>& entities,
      float confidence) const override {
    std::vector<Proposal> out;
    out.reserve(entities.size());
    for (const ModelInputEntity& e : entities) {
      float in[MlpModel::kIn];
      mlp_input(e.class_index, e.pos, e.vel, in);
      float delta[MlpModel::kOut];
      model_.forward(in, delta);
      Proposal p;
      p.producer = name();
      p.tick_observed = current;
      p.target_hint = e.id;
      p.position = Vec3{e.pos.x + delta[0], 0.0f, e.pos.y + delta[1]};
      p.confidence = confidence;
      out.push_back(std::move(p));
    }
    return out;
  }

  float train_loss() const { return train_loss_; }
  int n_samples() const { return n_samples_; }

  double last_fit_metric() const override { return train_loss_; }

 private:
  static constexpr int kEpochs = 30;
  static constexpr float kLr = 0.05f;
  static constexpr std::uint64_t kTrainSeed = 42;

  MlpModel model_{};
  std::vector<float> inputs_, targets_;
  int n_samples_ = 0;
  float train_loss_ = 0.0f;
};

}  // namespace

OW_REGISTER_WORLD_MODEL("mlp_dynamics", [] {
  return std::unique_ptr<WorldModelAdapter>(new MlpDynamicsModel());
});

}  // namespace ow
