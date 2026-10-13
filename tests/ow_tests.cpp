// Slice 01 tests — truth heartbeat. Zero dependencies; tiny check harness.
#include "core/hash.hpp"
#include "core/rng.hpp"
#include "truth_world/truth_world.hpp"
#include "world_state/world_state.hpp"

#include <cstdint>
#include <cstdio>

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

namespace {

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
  tw.spawn(ws, ClassTag{"x"}, Transform{}, Vec3{1.0f, 0.0f, 0.0f});
  tw.advance(ws, 10);
  tw.advance(ws, 3);  // backwards: defensive no-op, no crash, no state change
  CHECK(ws.tick() == 10);
}

}  // namespace

int main() {
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
