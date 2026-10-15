#include "loop/driver.hpp"

#include "adjudication/adjudication.hpp"
#include "belief/ledger.hpp"
#include "core/hash.hpp"
#include "metrics/metrics.hpp"
#include "observation/observation.hpp"
#include "perception/adapter.hpp"
#include "truth_world/truth_world.hpp"
#include "world_model/world_model.hpp"
#include "world_state/world_state.hpp"

#include <chrono>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace ow {
namespace {

using SteadyClock = std::chrono::steady_clock;

double ms_between(SteadyClock::time_point a, SteadyClock::time_point b) {
  return std::chrono::duration<double, std::milli>(b - a).count();
}

std::uint64_t hash_world(const WorldState& ws) {
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

std::uint64_t hash_ledger(const BeliefLedger& l) {
  Hasher h;
  for (const BeliefEntry& e : l.all()) {
    h.bytes(e.proposal.producer.data(), e.proposal.producer.size());
    h.value(e.proposal.tick_observed);
    h.value(static_cast<std::uint8_t>(e.proposal.target.index()));
    if (const auto* id = std::get_if<EntityId>(&e.proposal.target)) h.value(*id);
    h.value(e.proposal.fields.transform.has_value());
    if (e.proposal.fields.transform) {
      h.value(e.proposal.fields.transform->position);
    }
    h.value(e.proposal.confidence);
    h.value(static_cast<std::uint8_t>(e.verdict.kind));
    h.value(e.verdict.matched);
    h.bytes(e.verdict.reason.data(), e.verdict.reason.size());
    h.value(e.recorded_at);
  }
  return h.result();
}

}  // namespace

RunReport LoopDriver::run(const RunConfig& cfg) {
  RunReport rep;

  TruthWorld truth(cfg.seed);
  WorldState ws;
  truth.build_demo_scene(ws, scene_entities_);

  ObservationGenerator gen;
  NearestPositionStrategy strategy(0.1f);
  BeliefLedger ledger;
  MetricsModule metrics;
  const RenderConfig rcfg{cfg.width, cfg.height, cfg.footprint};

  std::vector<std::unique_ptr<PerceptionAdapter>> perceivers;
  for (const std::string& n : cfg.perception_adapters) {
    auto a = perception::make(n);
    if (a == nullptr) {
      throw std::invalid_argument("unknown perception adapter: " + n);
    }
    perceivers.push_back(std::move(a));
  }
  std::vector<std::unique_ptr<WorldModelAdapter>> models;
  for (const std::string& n : cfg.world_models) {
    auto m = world_model::make(n);
    if (m == nullptr) {
      throw std::invalid_argument("unknown world model: " + n);
    }
    models.push_back(std::move(m));
  }

  StageLatency lat_sum;
  const auto t_start = SteadyClock::now();
  for (Tick t = 1; t <= cfg.ticks; ++t) {
    TickLog log;
    log.tick = t;

    const auto s0 = SteadyClock::now();
    truth.advance(ws, t);
    const Observation obs = gen.render(ws, t, rcfg, Transform{});
    const auto s1 = SteadyClock::now();

    std::vector<Proposal> proposals;
    for (auto& a : perceivers) {
      try {  // isolation (slice 03 AC): a faulting adapter never halts ticks
        auto ps = a->observe(obs);
        proposals.insert(proposals.end(), std::make_move_iterator(ps.begin()),
                         std::make_move_iterator(ps.end()));
      } catch (...) {
        ++rep.adapter_faults;
      }
    }
    for (auto& m : models) {
      try {
        auto ps = m->predict(t, ws);
        proposals.insert(proposals.end(), std::make_move_iterator(ps.begin()),
                         std::make_move_iterator(ps.end()));
      } catch (...) {
        ++rep.adapter_faults;
      }
    }
    const auto s2 = SteadyClock::now();

    for (const Proposal& p : proposals) {
      const AdjudicationResult v = strategy.resolve(p, ws);
      switch (v.kind) {
        case VerdictKind::Matched: ++log.applied; break;
        case VerdictKind::NewEntity: ++log.announced; break;
        case VerdictKind::Rejected: ++log.rejected; break;
      }
      ledger.record(BeliefEntry{p, v, t});
    }
    const auto s3 = SteadyClock::now();

    metrics.feed(ws, ledger, t);
    const auto s4 = SteadyClock::now();

    log.latency.render_ms = ms_between(s0, s1);
    log.latency.perceive_ms = ms_between(s1, s2);
    log.latency.adjudicate_ms = ms_between(s2, s3);
    log.latency.metrics_ms = ms_between(s3, s4);
    lat_sum.render_ms += log.latency.render_ms;
    lat_sum.perceive_ms += log.latency.perceive_ms;
    lat_sum.adjudicate_ms += log.latency.adjudicate_ms;
    lat_sum.metrics_ms += log.latency.metrics_ms;

    rep.applied += log.applied;
    rep.announced += log.announced;
    rep.rejected += log.rejected;
  }
  const auto t_end = SteadyClock::now();

  rep.ticks_run = cfg.ticks;
  rep.total_seconds = std::chrono::duration<double>(t_end - t_start).count();
  rep.hz = rep.total_seconds > 0.0
               ? static_cast<double>(cfg.ticks) / rep.total_seconds
               : 0.0;
  rep.latency_sum = lat_sum;
  rep.metrics = metrics.report();
  rep.world_hash = hash_world(ws);
  rep.ledger_hash = hash_ledger(ledger);
  rep.ledger_entries = ledger.all().size();
  return rep;
}

}  // namespace ow
