// ow_tests — Grounding Loop v0 test binary.
//   ow_tests determinism   same seed => bit-identical digests (state, image,
//                          per-tick chain) across two full loop runs
//   ow_tests association   WorldState adjudication unit rules
//   ow_tests registry      adapter self-registration contract
#include "grounding_loop/loop.hpp"
#include "grounding_loop/phase1.hpp"
#include "models/mlp.hpp"
#include "models/rollout.hpp"
#include "models/world_model.hpp"
#include "observation/raster.hpp"
#include "perception/adapter.hpp"
#include "truth_world/truth_world.hpp"
#include "world_state/world_state.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {

int fail(const char* what) {
  std::printf("FAIL: %s\n", what);
  return 1;
}

int test_determinism() {
  ow::LoopConfig cfg;
  cfg.seed = 7;
  cfg.n_bodies = 8;
  cfg.ticks = 120;
  cfg.collect_per_tick_digests = true;

  const ow::LoopResult a = ow::run_grounding_loop(cfg);
  const ow::LoopResult b = ow::run_grounding_loop(cfg);

  if (a.per_tick_state_digests.size() != b.per_tick_state_digests.size())
    return fail("digest chain length mismatch");
  for (std::size_t i = 0; i < a.per_tick_state_digests.size(); ++i) {
    if (a.per_tick_state_digests[i] != b.per_tick_state_digests[i]) {
      std::printf("  tick %zu digest mismatch\n", i);
      return fail("per-tick state digest mismatch");
    }
  }
  if (a.final_state_digest != b.final_state_digest)
    return fail("final state digest mismatch");
  if (a.final_image_digest != b.final_image_digest)
    return fail("final image digest mismatch");
  // determinism must also break across seeds (sanity: seed matters)
  ow::LoopConfig other = cfg;
  other.seed = 8;
  const ow::LoopResult c = ow::run_grounding_loop(other);
  if (c.final_state_digest == a.final_state_digest)
    return fail("different seed produced identical state (RNG suspect)");
  std::printf("PASS determinism (state+image+chain, %d ticks)\n", cfg.ticks);
  return 0;
}

int test_association() {
  ow::WorldState ws;
  const ow::EntityId red = ws.mint("red");
  const ow::EntityId blue = ws.mint("blue");
  ws.commit_truth(1, {ow::TruthUpdate{red, {0.2f, 0, 0.2f}, {}},
                      ow::TruthUpdate{blue, {0.8f, 0, 0.8f}, {}}});

  // nearest match wins
  std::vector<ow::Proposal> props;
  ow::Proposal p;
  p.producer = "t";
  p.tick_observed = 1;
  p.class_tag = std::string("red");
  p.position = ow::Vec3{0.21f, 0.0f, 0.19f};
  p.confidence = 0.9f;
  props.push_back(p);
  auto res = ws.adjudicate(std::move(props), 0.06f);
  if (res.size() != 1 || !res[0].applied || res[0].entity != red)
    return fail("nearest-match rule");

  // class mismatch is rejected even when close
  props.clear();
  p.position = ow::Vec3{0.79f, 0.0f, 0.81f};
  p.class_tag = std::string("red");  // blue entity is nearest
  props.push_back(p);
  res = ws.adjudicate(std::move(props), 0.06f);
  if (res.size() != 1 || res[0].applied)
    return fail("class-mismatch rejection");

  // too far is rejected
  props.clear();
  p.position = ow::Vec3{0.5f, 0.0f, 0.5f};
  p.class_tag.reset();
  props.push_back(p);
  res = ws.adjudicate(std::move(props), 0.06f);
  if (res.size() != 1 || res[0].applied)
    return fail("threshold rejection");

  // belief samples recorded, owned fields untouched
  if (ws.belief_log().size() != 3) return fail("belief log size");
  if (ws.entities().at(red).position.x != 0.2f)
    return fail("belief write leaked into owned state");
  std::printf("PASS association (nearest+class+threshold+ownership)\n");
  return 0;
}

int test_registry() {
  const std::vector<std::string> names = ow::perception_names();
  bool has_cc = false, has_grid = false, has_bad = false;
  for (const auto& n : names) {
    if (n == "blob_cc") has_cc = true;
    if (n == "blob_grid") has_grid = true;
    if (n == "blob_cc_miscalibrated") has_bad = true;
  }
  if (!has_cc || !has_grid || !has_bad)
    return fail("expected adapters missing from registry");
  if (names.size() >= 2 && !(names[0] < names[names.size() - 1]))
    return fail("registry names not sorted");

  // every adapter produces proposals on a rendered frame
  ow::WorldState ws;
  ow::TruthWorld truth;
  truth.init(3, 8, ws);
  truth.step(ws);
  ow::Image img;
  render(truth.bodies(), img, truth.tick());
  const ow::TopDownCamera cam;
  for (const std::string& n : names) {
    auto a = ow::create_perception(n);
    if (!a) return fail("create_perception returned null");
    if (a->name() != n) return fail("adapter name mismatch");
    const auto props = a->detect(img, cam, truth.tick());
    if (props.empty()) return fail("adapter produced zero proposals");
    for (const auto& p : props) {
      if (!p.position.has_value() || !p.class_tag.has_value())
        return fail("proposal missing position/class");
      if (p.confidence < 0.0f || p.confidence > 1.0f)
        return fail("confidence out of range");
      if (p.producer != n) return fail("producer not self-reported");
    }
  }
  std::printf("PASS registry (%zu adapters self-registered, all detect)\n",
              names.size());
  return 0;
}

int test_phase1() {
  // world-model registry: the four Phase-1 models self-registered
  const std::vector<std::string> names = ow::world_model_names();
  bool has_replay = false, has_linear = false, has_zero = false, has_mlp = false;
  for (const auto& n : names) {
    if (n == "replay") has_replay = true;
    if (n == "linear") has_linear = true;
    if (n == "zero_delta") has_zero = true;
    if (n == "mlp_dynamics") has_mlp = true;
  }
  if (!has_replay || !has_linear || !has_zero || !has_mlp)
    return fail("expected world models missing from registry");

  // rollout recording is deterministic
  const ow::Rollout a = ow::record_rollout(7, 6, 50);
  const ow::Rollout b = ow::record_rollout(7, 6, 50);
  if (a.ticks.size() != b.ticks.size()) return fail("rollout length mismatch");
  for (std::size_t t = 0; t < a.ticks.size(); ++t) {
    const auto& ta = a.ticks[t];
    const auto& tb = b.ticks[t];
    if (ta.entities.size() != tb.entities.size())
      return fail("rollout entity count mismatch");
    for (std::size_t i = 0; i < ta.entities.size(); ++i) {
      const auto& ea = ta.entities[i];
      const auto& eb = tb.entities[i];
      if (ea.id != eb.id || ea.pos.x != eb.pos.x || ea.pos.y != eb.pos.y ||
          ea.vel.x != eb.vel.x || ea.vel.y != eb.vel.y)
        return fail("rollout bit-exact mismatch");
    }
  }

  // MLP training is deterministic (same data + seed => same weights)
  std::vector<float> inputs, targets;
  int n = 0;
  for (std::size_t t = 0; t + 1 < a.ticks.size(); ++t) {
    for (const auto& e : a.ticks[t].entities) {
      const ow::RolloutEntity* rec = nullptr;
      for (const auto& cand : a.ticks[t + 1].entities) {
        if (cand.id == e.id) { rec = &cand; break; }
      }
      if (!rec) continue;
      float in[ow::MlpModel::kIn];
      ow::mlp_input(e.class_index, e.pos, e.vel, in);
      for (float v : in) inputs.push_back(v);
      targets.push_back(rec->pos.x);
      targets.push_back(rec->pos.y);
      ++n;
    }
  }
  ow::MlpModel m1, m2;
  ow::mlp_train(m1, inputs, targets, n, 5, 0.05f, 42);
  ow::mlp_train(m2, inputs, targets, n, 5, 0.05f, 42);
  if (ow::mlp_weights_digest(m1) != ow::mlp_weights_digest(m2))
    return fail("MLP training not deterministic");
  ow::MlpModel m3;
  ow::mlp_train(m3, inputs, targets, n, 5, 0.05f, 43);
  if (ow::mlp_weights_digest(m1) == ow::mlp_weights_digest(m3))
    return fail("different training seed gave identical weights");

  // evaluation loop: oracle is exact; baselines ordered; MLP learns
  ow::Phase1Config cfg;
  cfg.seed = 7;
  cfg.n_bodies = 6;
  cfg.ticks = 120;
  const ow::Phase1Result r = ow::run_world_model_eval(cfg);
  const ow::ModelEval* replay = nullptr;
  const ow::ModelEval* linear = nullptr;
  const ow::ModelEval* zero = nullptr;
  const ow::ModelEval* mlp = nullptr;
  for (const auto& m : r.models) {
    if (m.model == "replay") replay = &m;
    if (m.model == "linear") linear = &m;
    if (m.model == "zero_delta") zero = &m;
    if (m.model == "mlp_dynamics") mlp = &m;
  }
  if (!replay || !linear || !zero || !mlp)
    return fail("phase1 eval missing models");
  if (replay->n_predictions == 0) return fail("replay produced no predictions");
  if (replay->mean_divergence != 0.0)
    return fail("replay oracle diverged (eval pipeline bug)");
  if (!(linear->mean_divergence < zero->mean_divergence))
    return fail("linear should beat zero_delta on a moving world");
  if (!(mlp->mean_divergence < zero->mean_divergence))
    return fail("MLP failed to beat the static-world floor");
  std::printf(
      "PASS phase1 (replay=0, linear=%.6f < zero=%.6f, mlp=%.6f < zero)\n",
      linear->mean_divergence, zero->mean_divergence, mlp->mean_divergence);
  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 2) {
    std::printf("usage: ow_tests <determinism|association|registry|phase1>\n");
    return 1;
  }
  const std::string t = argv[1];
  if (t == "determinism") return test_determinism();
  if (t == "association") return test_association();
  if (t == "registry") return test_registry();
  if (t == "phase1") return test_phase1();
  std::printf("unknown test: %s\n", t.c_str());
  return 1;
}
