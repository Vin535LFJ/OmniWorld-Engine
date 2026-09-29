#include "grounding_loop/loop.hpp"

#include "core/hash.hpp"
#include "observation/raster.hpp"
#include "truth_world/truth_world.hpp"

#include <algorithm>
#include <chrono>
#include <utility>

namespace ow {
namespace {
using clock_type = std::chrono::steady_clock;

double secs_since(clock_type::time_point t0) {
  return std::chrono::duration<double>(clock_type::now() - t0).count();
}
double micros_since(clock_type::time_point t0) {
  return std::chrono::duration<double, std::micro>(clock_type::now() - t0)
      .count();
}
}  // namespace

LoopResult run_grounding_loop(const LoopConfig& cfg) {
  LoopResult res;
  res.seed = cfg.seed;
  res.n_bodies = cfg.n_bodies;
  res.ticks = cfg.ticks;

  WorldState ws;
  TruthWorld truth;
  truth.init(cfg.seed, cfg.n_bodies, ws);

  auto adapters = create_all_perception();  // sorted by name
  for (const auto& a : adapters) res.adapters_used.push_back(a->name());

  std::map<std::string, ProducerMetrics> metrics;
  TopDownCamera cam;
  Image img;

  auto t_wall = clock_type::now();

  for (int t = 1; t <= cfg.ticks; ++t) {
    // 1) truth step (owner of kinematics)
    auto t0 = clock_type::now();
    truth.step(ws);
    res.stage_us["truth_step"].push_back(micros_since(t0));

    // 2) observation
    auto t1 = clock_type::now();
    render(truth.bodies(), img, truth.tick());
    res.stage_us["render"].push_back(micros_since(t1));

    // 3) perception adapters (see only RGB) + adjudication per producer
    for (const auto& adapter : adapters) {
      auto t2 = clock_type::now();
      auto proposals = adapter->detect(img, cam, truth.tick());
      const double detect_us = micros_since(t2);

      auto t3 = clock_type::now();
      auto results = ws.adjudicate(std::move(proposals), cfg.assoc_threshold);
      const double adj_us = micros_since(t3);

      res.stage_us["detect:" + adapter->name()].push_back(detect_us);
      res.stage_us["adjudicate:" + adapter->name()].push_back(adj_us);

      // 4) coherence metrics (id map is metrics-only ground truth)
      ProducerMetrics& m = metrics[adapter->name()];
      m.producer = adapter->name();
      const std::size_t n = results.size();
      for (std::size_t i = 0; i < n; ++i) {
        const Adjudication& a = results[i];
        // recover the proposal's pixel to find the true source entity
        const Vec3 pos = ws.belief_log().empty()
                             ? Vec3{}
                             : ws.belief_log()[ws.belief_log().size() - n + i].position;
        int px = static_cast<int>(pos.x * float(img.width));
        int py = static_cast<int>(pos.z * float(img.height));
        px = std::max(0, std::min(img.width - 1, px));
        py = std::max(0, std::min(img.height - 1, py));
        const EntityId true_id =
            img.ids[static_cast<std::size_t>(py) * img.width + px];

        ++m.n_proposals;
        if (!a.applied) {
          ++m.n_rejected;
          continue;
        }
        ++m.n_applied;
        const auto it = ws.entities().find(a.entity);
        if (it == ws.entities().end()) continue;
        const double err = static_cast<double>(
            distance(pos, it->second.position));
        m.sum_transform_error += err;
        ++m.n_err_samples;
        const bool correct = (true_id != 0) && (a.entity == true_id);
        if (correct) ++m.n_assoc_correct;
        const float conf =
            ws.belief_log()[ws.belief_log().size() - n + i].confidence;
        // ECE bins (applied proposals; "good" = correct id AND small error)
        int b = static_cast<int>(conf * 5.0f);
        if (b < 0) b = 0;
        if (b > 4) b = 4;
        m.bin_n[b] += 1;
        m.bin_conf_sum[b] += conf;
        if (correct && err <= 0.05) m.bin_good[b] += 1;
        if (conf >= 0.7f) {
          ++m.n_high_conf;
          if (!correct || err > 0.05) ++m.n_high_conf_wrong;
        }
      }
    }

    if (cfg.collect_per_tick_digests) {
      res.per_tick_state_digests.push_back(ws.state_digest());
    }
  }

  res.total_wall_s = secs_since(t_wall);
  res.loop_hz = res.total_wall_s > 0.0
                    ? double(cfg.ticks) / res.total_wall_s
                    : 0.0;

  // final digests
  res.final_state_digest = ws.state_digest();
  Hasher ih;
  ih.label("Image");
  ih.add_u64(static_cast<std::uint64_t>(img.tick));
  ih.add(img.rgb.data(), img.rgb.size());
  ih.add(img.ids.data(), img.ids.size() * sizeof(EntityId));
  res.final_image_digest = ih.h;

  for (auto& [name, m] : metrics) res.producers.push_back(m);  // map => sorted
  return res;
}

}  // namespace ow
