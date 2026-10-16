// ow_tests — zero-dependency check harness. Usage: ow_tests <slice-name>
// Slice granularity in ctest: one test name per slice, so a failure names its slice.
#include "core/hash.hpp"
#include "core/rng.hpp"
#include "adjudication/adjudication.hpp"
#include "belief/ledger.hpp"
#include "loop/driver.hpp"
#include "metrics/metrics.hpp"
#include "tolerance/tolerance.hpp"
#include "observation/observation.hpp"
#include "perception/adapter.hpp"
#include "truth_world/truth_world.hpp"
#include "world_model/world_model.hpp"
#include "world_state/world_state.hpp"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <variant>

namespace {
int g_failures = 0;
}  // namespace

#define CHECK(cond)                                                \
  do {                                                             \
    if (!(cond)) {                                                 \
      ++g_failures;                                                \
      std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);  \
    }                                                              \
  } while (0)

using namespace ow;

namespace slice01 {

std::uint64_t hash_state(const WorldState& ws) {
  Hasher h;
  h.value(ws.tick());
  for (const Entity& e : ws.entities()) {
    h.value(e.id);
    h.bytes(e.class_tag.value.data(), e.class_tag.value.size());
    h.value(e.transform.position);
    h.value(e.transform.orientation);
    h.value(e.velocity);
  }
  return h.result();
}

void test_tick_advance_exact() {
  TruthWorld tw(42);
  WorldState ws;
  const EntityId id = tw.spawn(ws, ClassTag{"blob"},
                               Transform{Vec3{0.0f, 0.0f, 0.0f}, Quat{}},
                               Vec3{0.25f, 0.0f, -0.5f});
  tw.advance(ws, 10);
  CHECK(ws.tick() == 10);
  const Entity* e = ws.find(id);
  CHECK(e != nullptr);
  if (e != nullptr) {
    CHECK(e->transform.position.x == 2.5f);   // 0.25 * 10, exact in binary
    CHECK(e->transform.position.y == 0.0f);
    CHECK(e->transform.position.z == -5.0f);  // -0.5 * 10, exact in binary
  }
}

void test_determinism() {
  TruthWorld a(7);
  WorldState wa;
  a.build_demo_scene(wa, 8);
  a.advance(wa, 100);

  TruthWorld b(7);
  WorldState wb;
  b.build_demo_scene(wb, 8);
  b.advance(wb, 100);

  CHECK(hash_state(wa) == hash_state(wb));  // same seed -> identical world

  TruthWorld c(8);
  WorldState wc;
  c.build_demo_scene(wc, 8);
  c.advance(wc, 100);
  CHECK(hash_state(wa) != hash_state(wc));  // different seed -> different world
}

void test_id_minting() {
  TruthWorld tw(1);
  WorldState ws;
  const EntityId a = tw.spawn(ws, ClassTag{"a"}, Transform{}, Vec3{});
  const EntityId b = tw.spawn(ws, ClassTag{"b"}, Transform{}, Vec3{});
  const EntityId c = tw.spawn(ws, ClassTag{"c"}, Transform{}, Vec3{});
  CHECK(a != 0 && b != 0 && c != 0);
  CHECK(a < b && b < c);  // monotonic, never reused
  CHECK(ws.size() == 3);
  CHECK(ws.find(a) != nullptr);
  CHECK(ws.find(999) == nullptr);
}

void test_rng_stable() {
  Rng a(1);
  Rng b(1);
  for (int i = 0; i < 5; ++i) {
    CHECK(a.next() == b.next());  // same seed -> same sequence
  }
  Rng c(2);
  const std::uint64_t first_of_2 = c.next();
  Rng d(1);
  CHECK(d.next() != first_of_2);  // different seeds -> different first draw
}

void test_advance_backwards_is_ignored() {
  TruthWorld tw(3);
  WorldState ws;
  tw.spawn(ws, ClassTag{"x"}, Transform{}, Vec3{});
  tw.advance(ws, 10);
  tw.advance(ws, 3);  // backwards: defensive no-op, no crash, no state change
  CHECK(ws.tick() == 10);
}

int run() {
  test_tick_advance_exact();
  test_determinism();
  test_id_minting();
  test_rng_stable();
  test_advance_backwards_is_ignored();
  if (g_failures == 0) {
    std::printf("slice01: all tests passed\n");
    return 0;
  }
  std::printf("slice01: %d check(s) FAILED\n", g_failures);
  return 1;
}

}  // namespace slice01

namespace slice02 {

// --- helpers -------------------------------------------------------------

// Expected palette color per the documented formula (tests recompute from
// the spec so the production formula gets pinned, not just exercised).
struct RGB { std::uint8_t r, g, b; };

RGB expected_color(const char* tag) {
  Hasher h;
  h.bytes(tag, std::strlen(tag));
  const std::uint64_t v = h.result();
  return {static_cast<std::uint8_t>(64 + (v & 0x7F)),
          static_cast<std::uint8_t>(64 + ((v >> 8) & 0x7F)),
          static_cast<std::uint8_t>(64 + ((v >> 16) & 0x7F))};
}

constexpr RGB kBg{0x10, 0x14, 0x18};

bool pixel_is(const Observation& o, int px, int py, RGB c) {
  const std::size_t idx = (static_cast<std::size_t>(py) * o.meta.width + px) * 3;
  return o.rgb[idx] == c.r && o.rgb[idx + 1] == c.g && o.rgb[idx + 2] == c.b;
}

bool frame_has_color(const Observation& o, RGB c) {
  for (std::size_t i = 0; i + 2 < o.rgb.size(); i += 3) {
    if (o.rgb[i] == c.r && o.rgb[i + 1] == c.g && o.rgb[i + 2] == c.b) return true;
  }
  return false;
}

std::uint64_t hash_observation(const Observation& o) {
  Hasher h;
  h.value(o.tick);
  h.bytes(o.meta.backend.data(), o.meta.backend.size());
  h.bytes(o.meta.device.data(), o.meta.device.size());
  h.bytes(o.meta.format.data(), o.meta.format.size());
  h.value(o.meta.width);
  h.value(o.meta.height);
  if (!o.rgb.empty()) h.bytes(o.rgb.data(), o.rgb.size());
  for (const auto& [id, v] : o.visibility) {
    h.value(id);
    h.value(static_cast<std::uint8_t>(v));
  }
  return h.result();
}

bool vis_is(const Observation& o, std::size_t i, EntityId id, Visibility v) {
  return o.visibility.size() > i && o.visibility[i].first == id &&
         o.visibility[i].second == v;
}

// --- behavior tests ------------------------------------------------------

void test_resolution_is_parameter() {
  TruthWorld tw(99);
  WorldState ws;
  const EntityId id =
      tw.spawn(ws, ClassTag{"blob"}, Transform{Vec3{0.5f, 0.0f, 0.5f}, Quat{}},
               Vec3{});
  ObservationGenerator gen;

  RenderConfig cfg8{8, 8, 0.25f};
  const Observation o8 = gen.render(ws, 5, cfg8, Transform{});
  CHECK(o8.meta.width == 8 && o8.meta.height == 8);
  CHECK(o8.rgb.size() == 8 * 8 * 3);
  CHECK(pixel_is(o8, 4, 4, expected_color("blob")));   // floor(0.5*8)=4, exact
  CHECK(pixel_is(o8, 0, 0, kBg));
  CHECK(vis_is(o8, 0, id, Visibility::Seen));

  RenderConfig cfg16{16, 16, 0.25f};
  const Observation o16 = gen.render(ws, 5, cfg16, Transform{});
  CHECK(o16.meta.width == 16 && o16.meta.height == 16);
  CHECK(o16.rgb.size() == 16 * 16 * 3);
  CHECK(pixel_is(o16, 8, 8, expected_color("blob")));  // floor(0.5*16)=8, exact
}

void test_three_state_visibility() {
  TruthWorld tw(5);
  WorldState ws;
  const EntityId a =
      tw.spawn(ws, ClassTag{"seen"}, Transform{Vec3{0.5f, 0.0f, 0.5f}, Quat{}},
               Vec3{});
  const EntityId b =
      tw.spawn(ws, ClassTag{"gone"}, Transform{Vec3{2.0f, 0.0f, 2.0f}, Quat{}},
               Vec3{});
  const EntityId c = tw.spawn(ws, ClassTag{"edge"},
                              Transform{Vec3{0.875f, 0.0f, 0.5f}, Quat{}},
                              Vec3{});
  ObservationGenerator gen;
  RenderConfig cfg{64, 64, 0.5f};
  const Observation o = gen.render(ws, 3, cfg, Transform{});

  CHECK(o.visibility.size() == 3);
  CHECK(vis_is(o, 0, a, Visibility::Seen));       // fully inside the window
  CHECK(vis_is(o, 1, b, Visibility::Unseen));     // fully outside
  CHECK(vis_is(o, 2, c, Visibility::Uncertain));  // straddles x=1 boundary

  // B is never drawn.
  CHECK(!frame_has_color(o, expected_color("gone")));
  // A covers columns 16..48 at row 32 (bbox [0.25,0.75] -> px 16..48).
  CHECK(pixel_is(o, 20, 32, expected_color("seen")));
  // C straddles: bbox [0.625,1.125] -> px 40..63; C is drawn after A (id order)
  // so its color wins in the overlap; column 63 shows C, column 20 shows A.
  CHECK(pixel_is(o, 63, 32, expected_color("edge")));
  CHECK(pixel_is(o, 20, 32, expected_color("seen")));
  CHECK(pixel_is(o, 0, 0, kBg));
}

void test_frame_metadata() {
  TruthWorld tw(1);
  WorldState ws;
  ObservationGenerator gen;
  RenderConfig cfg{4, 4, 0.25f};
  const Observation o = gen.render(ws, 42, cfg, Transform{});
  CHECK(o.tick == 42);
  CHECK(o.meta.backend == "cpu");
  CHECK(o.meta.device == "host");
  CHECK(o.meta.format == "RGB8");
  CHECK(o.sensor_kind == "rgb_topdown");
}

// --- boundary cases ------------------------------------------------------

void test_invalid_config_throws() {
  TruthWorld tw(1);
  WorldState ws;
  ObservationGenerator gen;
  bool caught = false;
  try {
    RenderConfig bad{0, 8, 0.25f};
    (void)gen.render(ws, 0, bad, Transform{});
  } catch (const std::invalid_argument&) {
    caught = true;
  }
  CHECK(caught);
}

void test_empty_world_is_legal() {
  TruthWorld tw(1);
  WorldState ws;
  ObservationGenerator gen;
  RenderConfig cfg{4, 4, 0.25f};
  const Observation o = gen.render(ws, 0, cfg, Transform{});
  CHECK(o.visibility.empty());
  CHECK(pixel_is(o, 0, 0, kBg));
  CHECK(o.rgb.size() == 4 * 4 * 3);
}

void test_boundary_straddler_does_not_crash() {
  TruthWorld tw(1);
  WorldState ws;
  tw.spawn(ws, ClassTag{"liminal"},
           Transform{Vec3{1.0f, 0.0f, 0.5f}, Quat{}}, Vec3{});
  ObservationGenerator gen;
  RenderConfig cfg{64, 64, 0.5f};
  const Observation o = gen.render(ws, 0, cfg, Transform{});
  // bbox [0.75, 1.25] straddles the x=1 boundary -> Uncertain; writes clamped.
  CHECK(o.visibility.size() == 1);
  CHECK(vis_is(o, 0, 1, Visibility::Uncertain));
}

// --- determinism, PAIRED with behavior (problem-log 002) ----------------

void test_determinism_pairing() {
  TruthWorld tw(7);
  WorldState ws;
  tw.spawn(ws, ClassTag{"a"}, Transform{Vec3{0.5f, 0.0f, 0.5f}, Quat{}},
           Vec3{});
  tw.spawn(ws, ClassTag{"b"}, Transform{Vec3{0.875f, 0.0f, 0.25f}, Quat{}},
           Vec3{0.125f, 0.0f, 0.0f});
  tw.spawn(ws, ClassTag{"c"}, Transform{Vec3{2.0f, 0.0f, 2.0f}, Quat{}},
           Vec3{});

  ObservationGenerator gen;
  RenderConfig cfg{64, 64, 0.5f};
  const Observation o1 = gen.render(ws, 10, cfg, Transform{});
  const Observation o2 = gen.render(ws, 10, cfg, Transform{});
  CHECK(hash_observation(o1) == hash_observation(o2));  // same input -> identical

  // Progress assertion (would fail on a trivially-deterministic stub):
  const Observation o3 = gen.render(ws, 11, cfg, Transform{});
  CHECK(hash_observation(o1) != hash_observation(o3));
}

int run() {
  test_resolution_is_parameter();
  test_three_state_visibility();
  test_frame_metadata();
  test_invalid_config_throws();
  test_empty_world_is_legal();
  test_boundary_straddler_does_not_crash();
  test_determinism_pairing();
  if (g_failures == 0) {
    std::printf("slice02: all tests passed\n");
    return 0;
  }
  std::printf("slice02: %d check(s) FAILED\n", g_failures);
  return 1;
}

}  // namespace slice02

namespace slice03 {

// --- helpers (self-contained per slice; duplicating the tiny color math
// is deliberate so each slice's expectations stand alone) ----------------

Observation blank_frame(int w, int h) {
  Observation o;
  o.tick = 0;
  o.meta.width = w;
  o.meta.height = h;
  o.rgb.assign(static_cast<std::size_t>(w) * h * 3, 0);
  for (int i = 0; i < w * h; ++i) {
    o.rgb[i * 3 + 0] = kBackgroundRgb[0];
    o.rgb[i * 3 + 1] = kBackgroundRgb[1];
    o.rgb[i * 3 + 2] = kBackgroundRgb[2];
  }
  return o;
}

void paint_blob(Observation& o, int px0, int py0, int px1, int py1,
                unsigned char r, unsigned char g, unsigned char b) {
  for (int py = py0; py <= py1; ++py) {
    for (int px = px0; px <= px1; ++px) {
      const std::size_t idx =
          (static_cast<std::size_t>(py) * o.meta.width + px) * 3;
      o.rgb[idx] = r;
      o.rgb[idx + 1] = g;
      o.rgb[idx + 2] = b;
    }
  }
}

bool proposal_eq(const Proposal& a, const Proposal& b) {
  if (a.producer != b.producer || a.tick_observed != b.tick_observed ||
      a.target.index() != b.target.index() || a.confidence != b.confidence)
    return false;
  if (a.fields.class_tag.has_value() != b.fields.class_tag.has_value())
    return false;
  if (a.fields.transform.has_value() != b.fields.transform.has_value())
    return false;
  if (a.fields.transform) {
    if (a.fields.transform->position.x != b.fields.transform->position.x)
      return false;
    if (a.fields.transform->position.z != b.fields.transform->position.z)
      return false;
  }
  return true;
}

// --- behavior tests ------------------------------------------------------

void test_registry_lookup() {
  const auto names = perception::names();
  CHECK(names.size() == 2);
  CHECK(names[0] == "blob_detect");  // std::map order = sorted = deterministic
  CHECK(names[1] == "blob_detect_overconfident");
  CHECK(perception::make("blob_detect") != nullptr);
  CHECK(perception::make("blob_detect_overconfident") != nullptr);
  CHECK(perception::make("does_not_exist") == nullptr);
}

void test_blob_centroid_exact() {
  Observation o = blank_frame(8, 8);
  o.tick = 9;
  paint_blob(o, 2, 4, 3, 5, 200, 60, 90);  // 2x2 at cols 2..3, rows 4..5
  auto ad = perception::make("blob_detect");
  const auto ps = ad->observe(o);
  CHECK(ps.size() == 1);
  if (ps.empty()) return;
  const Proposal& p = ps[0];
  CHECK(p.producer == "blob_detect");
  CHECK(p.tick_observed == 9);
  CHECK(std::holds_alternative<NewEntity>(p.target));
  CHECK(!p.fields.class_tag.has_value());  // adapters describe, never mint class
  CHECK(p.fields.transform.has_value());
  // centroid of the 2x2 block: px mean 2.5 -> x = (2.5+0.5)/8 = 0.375 (exact);
  // py mean 4.5 -> z = 0.625 (exact).
  CHECK(p.fields.transform->position.x == 0.375f);
  CHECK(p.fields.transform->position.z == 0.625f);
  CHECK(p.confidence > 0.0f && p.confidence <= 1.0f);
}

void test_two_blobs_two_proposals() {
  Observation o = blank_frame(16, 16);
  paint_blob(o, 2, 2, 2, 2, 255, 128, 0);
  paint_blob(o, 12, 12, 13, 13, 0, 128, 255);
  auto ad = perception::make("blob_detect");
  const auto ps = ad->observe(o);
  CHECK(ps.size() == 2);
  for (const auto& p : ps) {
    CHECK(p.producer == "blob_detect");
    CHECK(p.tick_observed == o.tick);
    CHECK(std::holds_alternative<NewEntity>(p.target));
  }
}

void test_overconfident_contrast() {
  Observation o = blank_frame(8, 8);
  paint_blob(o, 2, 2, 3, 3, 200, 60, 90);
  const auto honest = perception::make("blob_detect")->observe(o);
  const auto loud = perception::make("blob_detect_overconfident")->observe(o);
  CHECK(honest.size() == 1 && loud.size() == 1);
  if (honest.empty() || loud.empty()) return;
  CHECK(loud[0].confidence == 0.99f);  // always max-claims
  CHECK(honest[0].confidence < 0.99f && honest[0].confidence >= 0.70f);
}

void test_validate_proposal() {
  Proposal ok;
  ok.producer = "t";
  ok.confidence = 0.5f;
  CHECK(!validate_proposal(ok).has_value());
  Proposal p = ok;
  p.producer = "";
  CHECK(validate_proposal(p).has_value());
  p = ok;
  p.confidence = 1.5f;
  CHECK(validate_proposal(p).has_value());
  p.confidence = -0.1f;
  CHECK(validate_proposal(p).has_value());
  p.confidence = std::numeric_limits<float>::quiet_NaN();
  CHECK(validate_proposal(p).has_value());
}

void test_determinism_pairing() {
  Observation o = blank_frame(8, 8);
  paint_blob(o, 2, 4, 3, 5, 200, 60, 90);
  auto ad = perception::make("blob_detect");
  const auto a = ad->observe(o);
  const auto b = ad->observe(o);
  CHECK(a.size() == b.size());
  bool all_eq = a.size() == b.size();
  for (std::size_t i = 0; i < a.size(); ++i) all_eq = all_eq && proposal_eq(a[i], b[i]);
  CHECK(all_eq);
  paint_blob(o, 6, 6, 6, 6, 200, 60, 90);  // mutate -> proposals must change
  const auto c = ad->observe(o);
  CHECK(c.size() != a.size());
}

int run() {
  test_registry_lookup();
  test_blob_centroid_exact();
  test_two_blobs_two_proposals();
  test_overconfident_contrast();
  test_validate_proposal();
  test_determinism_pairing();
  if (g_failures == 0) {
    std::printf("slice03: all tests passed\n");
    return 0;
  }
  std::printf("slice03: %d check(s) FAILED\n", g_failures);
  return 1;
}

}  // namespace slice03

namespace slice04 {

// --- helpers (duplicated from slice01 on purpose: per-slice local) -------

std::uint64_t hash_state_s04(const WorldState& ws) {
  Hasher h;
  h.value(ws.tick());
  for (const Entity& e : ws.entities()) {
    h.value(e.id);
    h.bytes(e.class_tag.value.data(), e.class_tag.value.size());
    h.value(e.transform.position);
    h.value(e.transform.orientation);
    h.value(e.velocity);
  }
  return h.result();
}

Proposal pos_proposal(const char* producer, Tick t, float x, float z,
                      float conf) {
  Proposal p;
  p.producer = producer;
  p.tick_observed = t;
  p.target = NewEntity{};
  Transform tr;
  tr.position = Vec3{x, 0.0f, z};
  p.fields.transform = tr;
  p.confidence = conf;
  return p;
}

Proposal id_proposal(const char* producer, Tick t, EntityId id, float conf) {
  Proposal p;
  p.producer = producer;
  p.tick_observed = t;
  p.target = id;
  Transform tr;
  tr.position = Vec3{0.0f, 0.0f, 0.0f};
  p.fields.transform = tr;
  p.confidence = conf;
  return p;
}

struct Scene {
  TruthWorld tw;
  WorldState ws;
  EntityId e1, e2;
  Scene() : tw(1) {
    e1 = tw.spawn(ws, ClassTag{"a"},
                  Transform{Vec3{0.5f, 0.0f, 0.5f}, Quat{}}, Vec3{});
    e2 = tw.spawn(ws, ClassTag{"b"},
                  Transform{Vec3{0.25f, 0.0f, 0.25f}, Quat{}}, Vec3{});
  }
};

// --- behavior tests ------------------------------------------------------

void test_schema_rejection_carries_reason() {
  Scene s;
  NearestPositionStrategy strat;
  Proposal bad = pos_proposal("p", 0, 0.5f, 0.5f, 1.5f);
  const auto v = strat.resolve(bad, s.ws);
  CHECK(v.kind == VerdictKind::Rejected);
  CHECK(!v.reason.empty());
  Proposal no_producer = pos_proposal("", 0, 0.5f, 0.5f, 0.5f);
  const auto v2 = strat.resolve(no_producer, s.ws);
  CHECK(v2.kind == VerdictKind::Rejected);
  CHECK(!v2.reason.empty());
}

void test_explicit_target_match_and_unknown() {
  Scene s;
  NearestPositionStrategy strat;
  const auto hit = strat.resolve(id_proposal("p", 0, s.e1, 0.5f), s.ws);
  CHECK(hit.kind == VerdictKind::Matched && hit.matched == s.e1);
  const auto miss = strat.resolve(id_proposal("p", 0, 999, 0.5f), s.ws);
  CHECK(miss.kind == VerdictKind::Rejected && !miss.reason.empty());
}

void test_position_association() {
  Scene s;
  NearestPositionStrategy strat;  // tol = 0.1
  const auto near = strat.resolve(pos_proposal("p", 0, 0.52f, 0.5f, 0.5f), s.ws);
  CHECK(near.kind == VerdictKind::Matched && near.matched == s.e1);
  const auto far = strat.resolve(pos_proposal("p", 0, 0.9f, 0.9f, 0.5f), s.ws);
  CHECK(far.kind == VerdictKind::NewEntity && far.matched == 0);
}

void test_missing_position_rejected() {
  Scene s;
  NearestPositionStrategy strat;
  Proposal p;
  p.producer = "p";
  p.confidence = 0.5f;
  p.target = NewEntity{};  // no transform claimed
  const auto v = strat.resolve(p, s.ws);
  CHECK(v.kind == VerdictKind::Rejected);
  CHECK(v.reason.find("position") != std::string::npos);
}

void test_tie_breaks_by_id_order() {
  TruthWorld tw(7);
  WorldState ws;
  tw.spawn(ws, ClassTag{"a"}, Transform{Vec3{0.25f, 0.0f, 0.0f}, Quat{}}, Vec3{});
  tw.spawn(ws, ClassTag{"a"}, Transform{Vec3{0.75f, 0.0f, 0.0f}, Quat{}}, Vec3{});
  NearestPositionStrategy strat(0.5f);  // big tolerance
  const auto v = strat.resolve(pos_proposal("p", 0, 0.5f, 0.0f, 0.9f), ws);
  CHECK(v.kind == VerdictKind::Matched);
  CHECK(v.matched == 1);  // id order: first entity wins the tie
}

void test_strategy_is_replaceable_no_core_change() {
  struct AlwaysReject final : AssociationStrategy {
    AdjudicationResult resolve(const Proposal&, const WorldState&) const override {
      return {VerdictKind::Rejected, 0, "double"};
    }
  } always_reject;
  Scene s;
  const auto v =
      always_reject.resolve(pos_proposal("ok", 0, 0.5f, 0.5f, 0.5f), s.ws);
  CHECK(v.kind == VerdictKind::Rejected && v.reason == "double");
}

void test_truth_never_written() {
  Scene s;
  const std::uint64_t before = hash_state_s04(s.ws);
  NearestPositionStrategy strat;
  (void)strat.resolve(pos_proposal("p", 0, 0.52f, 0.5f, 0.5f), s.ws);
  (void)strat.resolve(pos_proposal("p", 0, 0.9f, 0.9f, 0.5f), s.ws);
  (void)strat.resolve(id_proposal("p", 0, 999, 0.5f), s.ws);
  const std::uint64_t after = hash_state_s04(s.ws);
  CHECK(before == after);  // adjudication must not mutate truth, ever
}

void test_ledger_append_only() {
  BeliefLedger ledger;
  BeliefEntry e1{pos_proposal("p", 0, 0.1f, 0.1f, 0.3f),
                 {VerdictKind::Matched, 1, ""}, 1};
  BeliefEntry e2{pos_proposal("p", 0, 0.2f, 0.2f, 0.4f),
                 {VerdictKind::NewEntity, 0, ""}, 1};
  ledger.record(e1);
  ledger.record(e2);
  BeliefEntry e3{pos_proposal("p", 0, 0.3f, 0.3f, 0.5f),
                 {VerdictKind::Rejected, 0, "r"}, 2};
  ledger.record(e3);
  CHECK(ledger.all().size() == 3);
  CHECK(ledger.at(1).size() == 2);
  CHECK(ledger.at(2).size() == 1);
  CHECK(ledger.at(99).empty());
  // append-only: earlier entries unchanged byte-for-byte after later appends
  CHECK(ledger.all()[0].proposal.confidence == 0.3f);
  CHECK(ledger.all()[0].verdict.matched == 1);
  CHECK(ledger.all()[0].recorded_at == 1);
}

void test_determinism_pairing() {
  Scene s;
  NearestPositionStrategy strat;
  const Proposal p = pos_proposal("p", 0, 0.52f, 0.5f, 0.5f);
  const auto a = strat.resolve(p, s.ws);
  const auto b = strat.resolve(p, s.ws);
  CHECK(a.kind == b.kind && a.matched == b.matched && a.reason == b.reason);
  // progress: a different claim associates to the OTHER entity
  const auto c = strat.resolve(pos_proposal("p", 0, 0.28f, 0.25f, 0.5f), s.ws);
  CHECK(c.matched != 0 && c.matched != s.e1);
}

int run() {
  test_schema_rejection_carries_reason();
  test_explicit_target_match_and_unknown();
  test_position_association();
  test_missing_position_rejected();
  test_tie_breaks_by_id_order();
  test_strategy_is_replaceable_no_core_change();
  test_truth_never_written();
  test_ledger_append_only();
  test_determinism_pairing();
  if (g_failures == 0) {
    std::printf("slice04: all tests passed\n");
    return 0;
  }
  std::printf("slice04: %d check(s) FAILED\n", g_failures);
  return 1;
}

}  // namespace slice04

namespace slice05 {

// --- helpers -------------------------------------------------------------

BeliefEntry matched_at(float x, float z, float conf, EntityId truth_id,
                       Tick t) {
  Proposal p;
  p.producer = "test";
  p.tick_observed = t;
  p.target = NewEntity{};
  Transform tr;
  tr.position = Vec3{x, 0.0f, z};
  p.fields.transform = tr;
  p.confidence = conf;
  return BeliefEntry{std::move(p),
                     AdjudicationResult{VerdictKind::Matched, truth_id, {}}, t};
}

struct World2 {
  TruthWorld tw{1};
  WorldState ws;
  EntityId e1, e2;
  World2() {
    e1 = tw.spawn(ws, ClassTag{"a"},
                  Transform{Vec3{0.5f, 0.0f, 0.5f}, Quat{}}, Vec3{});
    e2 = tw.spawn(ws, ClassTag{"b"},
                  Transform{Vec3{0.75f, 0.0f, 0.25f}, Quat{}}, Vec3{});
  }
};

// --- behavior tests ------------------------------------------------------

void test_defaults_present() {
  MetricsModule m;
  const auto r = m.report();
  CHECK(r.count("divergence") == 1);
  CHECK(r.count("ece") == 1);
  CHECK(r.count("identity_stability") == 1);
}

void test_divergence_exact() {
  World2 w;
  BeliefLedger l;
  // errors: |0.625-0.5|=0.125 and |0.75-0.5|=0.25 (both binary-exact)
  l.record(matched_at(0.625f, 0.5f, 0.875f, w.e1, 7));
  l.record(matched_at(0.75f, 0.5f, 0.875f, w.e1, 7));
  MetricsModule m;
  m.feed(w.ws, l, 7);
  const auto rep = m.report();
  CHECK(rep.at("divergence") == 0.1875);
}

void test_ece_discrimination_exact() {
  // Honest: conf 0.875 with acc 7/8 -> ECE 0. Overconfident: same errors with
  // conf 1.0 -> ECE 0.125. Gate-1c analog: the lie must surface, exactly.
  BeliefLedger honest, loud;
  for (int i = 0; i < 7; ++i) {  // 7 correct (err 0)
    honest.record(matched_at(0.5f, 0.5f, 0.875f, 1, 3));
    loud.record(matched_at(0.5f, 0.5f, 1.0f, 1, 3));
  }
  // 1 wrong (err 0.5 > tol 0.1)
  honest.record(matched_at(0.0f, 0.5f, 0.875f, 1, 3));
  loud.record(matched_at(0.0f, 0.5f, 1.0f, 1, 3));

  World2 w;
  MetricsModule ma, mb;
  ma.feed(w.ws, honest, 3);
  mb.feed(w.ws, loud, 3);
  const double ea = ma.report().at("ece");
  const double eb = mb.report().at("ece");
  CHECK(ea == 0.0);     // honest calibration: exact zero
  CHECK(eb == 0.125);   // the overconfidence tax, exact
  CHECK(eb > ea);
}

void test_identity_stability() {
  World2 w;
  {
    BeliefLedger l;  // tick1 {1,2}, tick2 {1,2}: perfectly tracked
    l.record(matched_at(0.5f, 0.5f, 0.9f, 1, 1));
    l.record(matched_at(0.75f, 0.25f, 0.9f, 2, 1));
    l.record(matched_at(0.5f, 0.5f, 0.9f, 1, 2));
    l.record(matched_at(0.75f, 0.25f, 0.9f, 2, 2));
    MetricsModule m;
    m.feed(w.ws, l, 1);
    m.feed(w.ws, l, 2);
    CHECK(m.report().at("identity_stability") == 1.0);
  }
  {
    BeliefLedger l;  // tick1 {1,2}, tick2 {1}: one entity lost -> 0
    l.record(matched_at(0.5f, 0.5f, 0.9f, 1, 1));
    l.record(matched_at(0.75f, 0.25f, 0.9f, 2, 1));
    l.record(matched_at(0.5f, 0.5f, 0.9f, 1, 2));
    MetricsModule m;
    m.feed(w.ws, l, 1);
    m.feed(w.ws, l, 2);
    CHECK(m.report().at("identity_stability") == 0.0);
  }
}

void test_metric_is_pluggable() {
  struct Const42 final : Metric {
    const char* name() const override { return "forty_two"; }
    void feed(const WorldState&, const BeliefLedger&, Tick) override {}
    double value() const override { return 42.5; }
  };
  MetricsModule m;
  m.register_metric(std::make_unique<Const42>());  // core untouched
  const auto r = m.report();
  CHECK(r.count("forty_two") == 1);
}

void test_determinism_pairing() {
  World2 w;
  BeliefLedger l;
  l.record(matched_at(0.625f, 0.5f, 0.875f, w.e1, 1));
  MetricsModule a, b;
  a.feed(w.ws, l, 1);
  b.feed(w.ws, l, 1);
  CHECK(a.report() == b.report());
  l.record(matched_at(0.0f, 0.5f, 0.875f, w.e1, 2));  // mutate -> must differ
  MetricsModule c;
  c.feed(w.ws, l, 1);
  c.feed(w.ws, l, 2);
  CHECK(!(c.report() == a.report()));
}

void test_empty_ledger_safe() {
  World2 w;
  BeliefLedger l;  // nothing recorded
  MetricsModule m;
  m.feed(w.ws, l, 1);
  const auto r = m.report();
  CHECK(r.at("divergence") == 0.0);
  CHECK(r.at("ece") == 0.0);
  CHECK(r.at("identity_stability") == 1.0);  // declared v1 semantic
}

int run() {
  test_defaults_present();
  test_divergence_exact();
  test_ece_discrimination_exact();
  test_identity_stability();
  test_metric_is_pluggable();
  test_determinism_pairing();
  test_empty_ledger_safe();
  if (g_failures == 0) {
    std::printf("slice05: all tests passed\n");
    return 0;
  }
  std::printf("slice05: %d check(s) FAILED\n", g_failures);
  return 1;
}

}  // namespace slice05

namespace slice06 {

RunConfig base_cfg() {
  RunConfig cfg{};
  cfg.ticks = 50;
  cfg.seed = 7;
  cfg.width = 32;
  cfg.height = 32;
  cfg.footprint = 0.02f;
  cfg.perception_adapters = {"blob_detect", "blob_detect_overconfident"};
  cfg.world_models = {"zero_delta", "linear_extrapolation"};
  return cfg;
}

void test_world_model_registry() {
  const auto names = world_model::names();
  CHECK(names.size() == 2);
  CHECK(names[0] == "linear_extrapolation");
  CHECK(names[1] == "zero_delta");
  CHECK(world_model::make("zero_delta") != nullptr);
  CHECK(world_model::make("linear_extrapolation") != nullptr);
  CHECK(world_model::make("nope") == nullptr);
}

void test_models_predict_exactly() {
  TruthWorld tw(1);
  WorldState ws;
  const EntityId e1 = tw.spawn(ws, ClassTag{"a"},
                               Transform{Vec3{0.5f, 0.0f, 0.5f}, Quat{}},
                               Vec3{0.25f, 0.0f, 0.0f});
  auto lin = world_model::make("linear_extrapolation");
  const auto lp = lin->predict(4, ws);
  CHECK(lp.size() == 1);
  if (!lp.empty()) {
    CHECK(std::holds_alternative<EntityId>(lp[0].target));
    CHECK(std::get<EntityId>(lp[0].target) == e1);
    CHECK(lp[0].fields.transform.has_value());
    CHECK(lp[0].fields.transform->position.x == 0.75f);  // 0.5+0.25 exact
    CHECK(lp[0].confidence == 0.5f);                     // v0.1 as-built habit
    CHECK(lp[0].tick_observed == 4);
  }
  auto zero = world_model::make("zero_delta");
  const auto zp = zero->predict(4, ws);
  CHECK(zp.size() == 1);
  if (!zp.empty()) {
    CHECK(zp[0].fields.transform->position.x == 0.5f);  // stays put
  }
}

void test_loop_end_to_end() {
  LoopDriver loop(8);
  const RunReport r = loop.run(base_cfg());
  CHECK(r.ticks_run == 50);
  CHECK(r.ledger_entries > 0);
  CHECK(r.metrics.count("divergence") == 1);
  CHECK(r.metrics.count("ece") == 1);
  CHECK(r.metrics.count("identity_stability") == 1);
  CHECK(r.applied > 0);
  CHECK(r.applied + r.announced + r.rejected == r.ledger_entries);
}

void test_loop_determinism_pairing() {
  LoopDriver loop(8);
  const RunReport a = loop.run(base_cfg());
  const RunReport b = loop.run(base_cfg());
  CHECK(a.world_hash == b.world_hash);
  CHECK(a.ledger_hash == b.ledger_hash);
  CHECK(a.metrics == b.metrics);  // doubles compared exactly
  RunConfig other = base_cfg();
  other.seed = 8;
  const RunReport c = loop.run(other);
  CHECK(c.world_hash != a.world_hash);  // progress: seed actually matters
}

void test_adapter_fault_isolation() {
  struct ThrowingPerception final : PerceptionAdapter {
    const char* name() const override { return "throwing_test"; }
    std::vector<Proposal> observe(const Observation&) override {
      throw std::runtime_error("adapter boom");
    }
  };
  perception::reg("throwing_test", []() -> std::unique_ptr<PerceptionAdapter> {
    return std::make_unique<ThrowingPerception>();
  });
  RunConfig cfg = base_cfg();
  cfg.perception_adapters = {"blob_detect", "throwing_test"};
  LoopDriver loop(8);
  const RunReport r = loop.run(cfg);
  CHECK(r.ticks_run == 50);       // kept ticking...
  CHECK(r.adapter_faults >= 50);  // ...while counting the fault every tick
  CHECK(r.ledger_entries > 0);    // other adapters unaffected
}

int run() {
  test_world_model_registry();
  test_models_predict_exactly();
  test_loop_end_to_end();
  test_loop_determinism_pairing();
  test_adapter_fault_isolation();
  if (g_failures == 0) {
    std::printf("slice06: all tests passed\n");
    return 0;
  }
  std::printf("slice06: %d check(s) FAILED\n", g_failures);
  return 1;
}

}  // namespace slice06

namespace slice07 {

RunConfig mini_cfg() {
  RunConfig cfg{};
  cfg.ticks = 8;
  cfg.seed = 7;
  cfg.width = 32;
  cfg.height = 32;
  cfg.footprint = 0.02f;
  cfg.perception_adapters = {"blob_detect"};
  cfg.world_models = {"zero_delta"};
  cfg.mode = RunMode::Reference;
  return cfg;
}

void test_parse_defaults() {
  const char* argv[] = {"prog"};
  std::string out;
  const auto parsed = parse_config(1, argv, out);
  CHECK(std::holds_alternative<RunConfig>(parsed));
  if (std::holds_alternative<RunConfig>(parsed)) {  // CLI-layer defaults
    const RunConfig& c = std::get<RunConfig>(parsed);
    CHECK(c.mode == RunMode::Reference);
    CHECK(c.ticks == 2000);
    CHECK(c.width == 64 && c.height == 64);
    CHECK(c.footprint == 0.02f);
    CHECK(c.perception_adapters.size() == 2);
    CHECK(c.world_models.size() == 2);
    CHECK(out.empty());
  }
}

void test_parse_mode_and_values() {
  const char* argv[] = {"prog", "--mode", "performance", "--ticks", "100",
                        "--seed", "9", "--adapters", "blob_detect", "--out", "x.json"};
  std::string out;
  const auto parsed = parse_config(11, argv, out);
  CHECK(std::holds_alternative<RunConfig>(parsed));
  if (std::holds_alternative<RunConfig>(parsed)) {
    const RunConfig& c = std::get<RunConfig>(parsed);
    CHECK(c.mode == RunMode::Performance);
    CHECK(c.ticks == 100);
    CHECK(c.seed == 9);
    CHECK(c.perception_adapters.size() == 1);
    CHECK(out == "x.json");
  }
}

void test_parse_fail_fast() {
  std::string out;
  const char* bad_mode[] = {"prog", "--mode", "bogus"};
  const auto e1 = parse_config(3, bad_mode, out);
  CHECK(std::holds_alternative<std::string>(e1));
  if (std::holds_alternative<std::string>(e1)) {
    CHECK(std::get<std::string>(e1).find("mode") != std::string::npos);
  }
  const char* unknown[] = {"prog", "--wat"};
  const auto e2 = parse_config(2, unknown, out);
  CHECK(std::holds_alternative<std::string>(e2));
}

void test_json_mode_and_env_fields() {
  LoopDriver loop(4);
  RunReport r = loop.run(mini_cfg());
  const std::string j = run_report_json(r);
  CHECK(j.find("\"mode\": \"reference\"") != std::string::npos);
  CHECK(j.find("\"acceleration\": \"none\"") != std::string::npos);
  CHECK(j.find("\"os\": \"windows\"") != std::string::npos);
  CHECK(j.find("\"compiler\": \"MSVC ") != std::string::npos);
  CHECK(j.find("\"git_commit\": ") != std::string::npos);
  CHECK(j.find("\"world_hash\": \"") != std::string::npos);
}

void test_json_is_deterministic() {
  LoopDriver loop(4);
  const RunReport r = loop.run(mini_cfg());
  CHECK(run_report_json(r) == run_report_json(r));  // byte-identical (no wall clock)
}

void test_modes_share_one_path_v1() {
  LoopDriver loop(4);
  const RunReport a = loop.run(mini_cfg());
  RunConfig perf = mini_cfg();
  perf.mode = RunMode::Performance;
  const RunReport b = loop.run(perf);
  CHECK(a.world_hash == b.world_hash);    // v1: same path, honest labeling
  CHECK(a.ledger_hash == b.ledger_hash);
  const std::string jb = run_report_json(b);
  CHECK(jb.find("\"mode\": \"performance\"") != std::string::npos);
  CHECK(jb.find("no GPU components yet") != std::string::npos);
}

int run() {
  test_parse_defaults();
  test_parse_mode_and_values();
  test_parse_fail_fast();
  test_json_mode_and_env_fields();
  test_json_is_deterministic();
  test_modes_share_one_path_v1();
  if (g_failures == 0) {
    std::printf("slice07: all tests passed\n");
    return 0;
  }
  std::printf("slice07: %d check(s) FAILED\n", g_failures);
  return 1;
}

}  // namespace slice07

namespace slice08 {

// Test-local noise stub: a world-model adapter with a parameterized x-axis
// offset (precedent: slice04 AlwaysReject / slice06 ThrowingPerception — it
// never enters src/, it is the tolerance window's own proof instrument).
struct NoisyPredictor final : WorldModelAdapter {
  explicit NoisyPredictor(float eps) : eps_(eps) {}
  const char* name() const override { return "noise_stub"; }
  std::vector<Proposal> predict(Tick t, const WorldState& ws) override {
    std::vector<Proposal> out;
    for (const Entity& e : ws.entities()) {
      Proposal p;
      p.producer = "noise_stub";
      p.tick_observed = t;
      p.target = e.id;
      Transform tr;
      tr.position = Vec3{e.transform.position.x + eps_,
                         e.transform.position.y,
                         e.transform.position.z};
      p.fields.transform = tr;
      p.confidence = 0.5f;
      out.push_back(std::move(p));
    }
    return out;
  }

 private:
  float eps_;
};

float kEpsSmall = 0.02f;
float kEpsBig = 0.2f;
double kDivTol = 0.05;  // calibrated once against the real deltas below

void test_comparator_pure_function() {
  const std::map<std::string, double> a{{"divergence", 0.1}, {"ece", 0.2}};
  const std::map<std::string, double> b{{"divergence", 0.11}, {"ece", 0.2}};
  const std::vector<ToleranceWindow> w{{"divergence", 0.02}, {"ece", 0.02}};
  const ToleranceVerdict v = compare_within_tolerance(a, b, w);
  CHECK(v.pass);
  CHECK(v.deltas.size() == 2);
  CHECK(v.deltas[0].first == "divergence");
  CHECK(v.deltas[0].second == 0.11 - 0.1);  // same expression, same double
  CHECK(v.deltas[1].second == 0.0);

  // Boundary: delta exactly == window passes (binary-exact values).
  const std::map<std::string, double> c{{"d", 0.25}};
  const std::map<std::string, double> d{{"d", 0.5}};
  const ToleranceVerdict vb =
      compare_within_tolerance(c, d, {{"d", 0.25}});
  CHECK(vb.pass);

  // Missing key: sentinel -1.0 and fail.
  const std::map<std::string, double> e{{"other", 1.0}};
  const ToleranceVerdict vm = compare_within_tolerance(c, e, {{"d", 1.0}});
  CHECK(!vm.pass);
  CHECK(vm.deltas[0].second == kMissingDelta);

  // Empty windows: nothing to compare.
  const ToleranceVerdict ve = compare_within_tolerance(a, b, {});
  CHECK(ve.pass);
  CHECK(ve.deltas.empty());

  // NaN metric values fail their window without a special case.
  const std::map<std::string, double> n{{"d", std::nan("")}};
  const ToleranceVerdict vn =
      compare_within_tolerance(c, n, {{"d", 1.0}});
  CHECK(!vn.pass);
}

void test_stub_registration() {
  CHECK(world_model::make("noise_stub_small") != nullptr);
  CHECK(world_model::make("noise_stub_big") != nullptr);
  auto small = world_model::make("noise_stub_small");
  TruthWorld tw(1);
  WorldState ws;
  tw.spawn(ws, ClassTag{"a"}, Transform{Vec3{0.5f, 0.0f, 0.5f}, Quat{}},
           Vec3{});
  const auto ps = small->predict(3, ws);
  CHECK(ps.size() == 1);
  if (!ps.empty()) {
    CHECK(ps[0].producer == "noise_stub");
    CHECK(ps[0].fields.transform->position.x ==
          0.5f + kEpsSmall);  // offset visible
  }
}

RunConfig tolerance_cfg() {
  RunConfig cfg{};
  cfg.ticks = 50;
  cfg.seed = 42;
  cfg.width = 64;
  cfg.height = 64;
  cfg.footprint = 0.02f;
  cfg.perception_adapters = {};  // mechanism isolation: no perception noise
  cfg.world_models = {"zero_delta"};
  cfg.mode = RunMode::Reference;
  return cfg;
}

void test_self_proof_both_directions() {
  LoopDriver loop(8);
  const RunReport base = loop.run(tolerance_cfg());

  RunConfig small = tolerance_cfg();
  small.world_models = {"noise_stub_small"};
  const RunReport rs = loop.run(small);

  RunConfig big = tolerance_cfg();
  big.world_models = {"noise_stub_big"};
  const RunReport rb = loop.run(big);

  const std::vector<ToleranceWindow> window{{"divergence", kDivTol}};
  const ToleranceVerdict within =
      compare_within_tolerance(base.metrics, rs.metrics, window);
  CHECK(within.pass);  // small noise: inside the declared window

  const ToleranceVerdict beyond =
      compare_within_tolerance(base.metrics, rb.metrics, window);
  CHECK(!beyond.pass);  // big noise: outside, and the deltas say why
  CHECK(!beyond.deltas.empty());
  CHECK(beyond.deltas[0].first == "divergence");
  CHECK(beyond.deltas[0].second > kDivTol);
}

void test_determinism_pairing() {
  const std::map<std::string, double> a{{"d", 0.1}};
  const std::map<std::string, double> b{{"d", 0.11}};
  const std::vector<ToleranceWindow> w{{"d", 0.02}};
  const ToleranceVerdict v1 = compare_within_tolerance(a, b, w);
  const ToleranceVerdict v2 = compare_within_tolerance(a, b, w);
  CHECK(v1.pass == v2.pass && v1.deltas == v2.deltas);

  LoopDriver loop(4);
  RunConfig small = tolerance_cfg();
  small.ticks = 4;
  small.world_models = {"noise_stub_small"};
  const RunReport r1 = loop.run(small);
  const RunReport r2 = loop.run(small);
  CHECK(r1.metrics == r2.metrics);  // same noise, same metrics: loop-side determinism
}

int run() {
  world_model::reg("noise_stub_small", []() -> std::unique_ptr<WorldModelAdapter> {
    return std::make_unique<NoisyPredictor>(kEpsSmall);
  });
  world_model::reg("noise_stub_big", []() -> std::unique_ptr<WorldModelAdapter> {
    return std::make_unique<NoisyPredictor>(kEpsBig);
  });
  test_comparator_pure_function();
  test_stub_registration();
  test_self_proof_both_directions();
  test_determinism_pairing();
  if (g_failures == 0) {
    std::printf("slice08: all tests passed\n");
    return 0;
  }
  std::printf("slice08: %d check(s) FAILED\n", g_failures);
  return 1;
}

}  // namespace slice08

namespace slice09 {

void test_percentile_known_samples() {
  const StageSamples s{{"x", {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}}};
  const auto p = percentiles(s);
  CHECK(p.count("x") == 1);
  CHECK(p.at("x").p50 == 5.5);  // rank 4.5 -> exact
  // Rank-linear interpolation written out so the double matches bit-for-bit.
  CHECK(p.at("x").p95 == 9.0 + (0.95 * 9.0 - 8.0) * (10.0 - 9.0));
  const StageSamples one{{"y", {2.5}}};
  const auto p1 = percentiles(one);
  CHECK(p1.at("y").p50 == 2.5);
  CHECK(p1.at("y").p95 == 2.5);  // single sample: the value itself
  const StageSamples none{{"z", {}}};
  const auto p0 = percentiles(none);
  CHECK(p0.at("z").p50 == 0.0);
  CHECK(p0.at("z").p95 == 0.0);
}

RunConfig lat_cfg() {
  RunConfig cfg{};
  cfg.ticks = 20;
  cfg.seed = 42;
  cfg.width = 64;
  cfg.height = 64;
  cfg.footprint = 0.02f;
  cfg.perception_adapters = {"blob_detect"};
  cfg.world_models = {"linear_extrapolation"};
  cfg.mode = RunMode::Reference;
  return cfg;
}

void test_samples_and_keys() {
  LoopDriver loop(4);
  const RunReport r = loop.run(lat_cfg());
  CHECK(r.stage_samples.count("truth_step") == 1);
  CHECK(r.stage_samples.count("render") == 1);
  CHECK(r.stage_samples.count("detect:blob_detect") == 1);
  CHECK(r.stage_samples.count("predict:linear_extrapolation") == 1);
  CHECK(r.stage_samples.count("adjudicate") == 1);
  CHECK(r.stage_samples.count("metrics") == 1);
  for (const auto& [k, v] : r.stage_samples) {
    CHECK(v.size() == static_cast<std::size_t>(20));  // one sample per tick
  }
  CHECK(r.stage_percentiles.size() == r.stage_samples.size());
  const auto keys = latency_keys(r);
  CHECK(keys.count("render.p50") == 1);
  CHECK(keys.count("detect:blob_detect.p95") == 1);
}

void test_budget_mount() {
  LoopDriver loop(4);
  const RunReport r = loop.run(lat_cfg());
  const auto keys = latency_keys(r);
  // Note: truth_step (one advance of 8 entities) often measures 0.0us at this
  // machine's timer resolution — a heavy stage demonstrates the mount instead.
  const double v = keys.at("render.p50");
  CHECK(v > 0.0);  // filling a 64x64 frame costs real microseconds
  // Budget semantics: compare the report's keys against a declared budget map.
  const std::map<std::string, double> budget{{"render.p50", v}};
  const std::vector<ToleranceWindow> generous{{"render.p50", v}};
  CHECK(compare_within_tolerance(keys, budget, generous).pass);  // delta 0
  const std::map<std::string, double> zero_budget{{"render.p50", 0.0}};
  const std::vector<ToleranceWindow> zero{{"render.p50", 0.0}};
  CHECK(!compare_within_tolerance(keys, zero_budget, zero).pass);  // v > 0
  const std::vector<ToleranceWindow> ghost{{"nope", 1.0}};
  const auto vm = compare_within_tolerance(keys, keys, ghost);
  CHECK(!vm.pass);
  CHECK(vm.deltas[0].second == kMissingDelta);
}

void test_json_and_wallclock_regression() {
  LoopDriver loop(4);
  const RunReport a = loop.run(lat_cfg());
  const RunReport b = loop.run(lat_cfg());
  CHECK(a.world_hash == b.world_hash);    // timing never leaks into hashes
  CHECK(a.ledger_hash == b.ledger_hash);
  const std::string j = run_report_json(a);
  CHECK(j.find("\"stage_percentiles\"") != std::string::npos);
  CHECK(j.find("\"render\"") != std::string::npos);
  CHECK(j.find("\"p50\"") != std::string::npos);
  CHECK(run_report_json(a) == run_report_json(a));  // same report: byte-identical
}

int run() {
  test_percentile_known_samples();
  test_samples_and_keys();
  test_budget_mount();
  test_json_and_wallclock_regression();
  if (g_failures == 0) {
    std::printf("slice09: all tests passed\n");
    return 0;
  }
  std::printf("slice09: %d check(s) FAILED\n", g_failures);
  return 1;
}

}  // namespace slice09

namespace slice10 {

BeliefEntry ece_entry(const char* producer, EntityId target_id, double x,
                      double z, float conf, Tick t) {
  Proposal p;
  p.producer = producer;
  p.tick_observed = t;
  p.target = target_id;
  Transform tr;
  tr.position = Vec3{static_cast<float>(x), 0.0f, static_cast<float>(z)};
  p.fields.transform = tr;
  p.confidence = conf;
  return BeliefEntry{std::move(p),
                     AdjudicationResult{VerdictKind::Matched, target_id, {}}, t};
}

// Feed n proposals: `correct_first` right (position = truth) then the rest
// wrong (position = truth + 0.5 on x, beyond the 0.1 correctness bar).
void feed_pattern(MetricsModule& m, const WorldState& ws, const char* producer,
                  int correct_first, int wrong_after, float conf) {
  BeliefLedger l;
  for (int i = 0; i < correct_first; ++i) {
    l.record(ece_entry(producer, 1, 0.5, 0.5, conf, 1));
  }
  for (int i = 0; i < wrong_after; ++i) {
    l.record(ece_entry(producer, 1, 1.0, 0.5, conf, 1));
  }
  m.feed(ws, l, 1);
}

void test_decomposed_exact_values() {
  TruthWorld tw(1);
  WorldState ws;
  tw.spawn(ws, ClassTag{"a"}, Transform{Vec3{0.5f, 0.0f, 0.5f}, Quat{}},
           Vec3{});

  MetricsModule m;  // two producers, same accuracy structure, different claims
  feed_pattern(m, ws, "honest", 7, 1, 0.875f);   // conf == accuracy
  feed_pattern(m, ws, "loud", 7, 1, 1.0f);       // overclaims the same errors

  const auto r = m.report();
  CHECK(r.count("ece") == 1);
  CHECK(r.count("ece:honest") == 1);
  CHECK(r.count("ece:loud") == 1);
  CHECK(r.at("ece:honest") == 0.0);    // calibrated: exact zero
  CHECK(r.at("ece:loud") == 0.125);    // |7/8 - 1.0|, exact
  CHECK(r.at("ece") == 0.0625);        // (0 + 0.125) / 2, equal counts
}

void test_aggregate_between_decomposed() {
  LoopDriver loop(4);
  RunConfig cfg{};
  cfg.ticks = 30;
  cfg.seed = 42;
  cfg.width = 64;
  cfg.height = 64;
  cfg.footprint = 0.02f;
  cfg.perception_adapters = {"blob_detect", "blob_detect_overconfident"};
  cfg.world_models = {};  // perception-only: the aggregate then sits between the two detectors
  cfg.mode = RunMode::Reference;
  const auto r = loop.run(cfg).metrics;
  CHECK(r.count("ece") == 1);
  CHECK(r.count("ece:blob_detect") == 1);
  CHECK(r.count("ece:blob_detect_overconfident") == 1);
  const double hi = r.at("ece:blob_detect");
  const double lo = r.at("ece:blob_detect_overconfident");
  const double agg = r.at("ece");
  CHECK(hi > lo + 0.05);  // honest detector under-claims vs over-claiming one
  CHECK(agg >= lo && agg <= hi);  // aggregate stays inside the decomposed range
}

int run() {
  test_decomposed_exact_values();
  test_aggregate_between_decomposed();
  if (g_failures == 0) {
    std::printf("slice10: all tests passed\n");
    return 0;
  }
  std::printf("slice10: %d check(s) FAILED\n", g_failures);
  return 1;
}

}  // namespace slice10

int main(int argc, char** argv) {
  if (argc < 2) {
    std::printf("usage: ow_tests <slice-name>\n");
    return 1;
  }
  if (std::strcmp(argv[1], "slice01") == 0) return slice01::run();
  if (std::strcmp(argv[1], "slice02") == 0) return slice02::run();
  if (std::strcmp(argv[1], "slice03") == 0) return slice03::run();
  if (std::strcmp(argv[1], "slice04") == 0) return slice04::run();
  if (std::strcmp(argv[1], "slice05") == 0) return slice05::run();
  if (std::strcmp(argv[1], "slice06") == 0) return slice06::run();
  if (std::strcmp(argv[1], "slice07") == 0) return slice07::run();
  if (std::strcmp(argv[1], "slice08") == 0) return slice08::run();
  if (std::strcmp(argv[1], "slice09") == 0) return slice09::run();
  if (std::strcmp(argv[1], "slice10") == 0) return slice10::run();
  std::printf("unknown slice: %s\n", argv[1]);
  return 1;
}
