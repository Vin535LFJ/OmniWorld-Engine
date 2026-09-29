// ow_tests — Grounding Loop v0 test binary.
//   ow_tests determinism   same seed => bit-identical digests (state, image,
//                          per-tick chain) across two full loop runs
//   ow_tests association   WorldState adjudication unit rules
//   ow_tests registry      adapter self-registration contract
#include "grounding_loop/loop.hpp"
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

}  // namespace

int main(int argc, char** argv) {
  if (argc < 2) {
    std::printf("usage: ow_tests <determinism|association|registry>\n");
    return 1;
  }
  const std::string t = argv[1];
  if (t == "determinism") return test_determinism();
  if (t == "association") return test_association();
  if (t == "registry") return test_registry();
  std::printf("unknown test: %s\n", t.c_str());
  return 1;
}
