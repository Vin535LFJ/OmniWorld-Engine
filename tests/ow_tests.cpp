// ow_tests — zero-dependency check harness. Usage: ow_tests <slice-name>
// Slice granularity in ctest: one test name per slice, so a failure names its slice.
#include "core/hash.hpp"
#include "core/rng.hpp"
#include "observation/observation.hpp"
#include "truth_world/truth_world.hpp"
#include "world_state/world_state.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <stdexcept>

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

int main(int argc, char** argv) {
  if (argc < 2) {
    std::printf("usage: ow_tests <slice-name>\n");
    return 1;
  }
  if (std::strcmp(argv[1], "slice01") == 0) return slice01::run();
  if (std::strcmp(argv[1], "slice02") == 0) return slice02::run();
  std::printf("unknown slice: %s\n", argv[1]);
  return 1;
}
