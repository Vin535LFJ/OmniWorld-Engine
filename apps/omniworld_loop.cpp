// omniworld_loop — runner for both loop stages:
//   phase0 (default): Grounding Loop v0 — Gate 1 (PLAN.md section 6)
//   phase1: world models in the loop — prediction-vs-truth divergence
#include "grounding_loop/loop.hpp"
#include "grounding_loop/phase1.hpp"
#include "models/rollout.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {

double percentile(std::vector<double> v, double p) {
  if (v.empty()) return 0.0;
  std::sort(v.begin(), v.end());
  double idx = p * double(v.size() - 1);
  size_t lo = size_t(idx);
  size_t hi = std::min(lo + 1, v.size() - 1);
  double frac = idx - double(lo);
  return v[lo] * (1.0 - frac) + v[hi] * frac;
}

struct Args {
  std::uint64_t seed = 42;
  int ticks = 600;
  int bodies = 12;
  std::string mode = "phase0";  // phase0 | phase1
  std::string out;              // optional artifacts dir for the json report
};

Args parse_args(int argc, char** argv) {
  Args a;
  for (int i = 1; i < argc; ++i) {
    const std::string k = argv[i];
    auto next = [&]() -> std::string {
      return (i + 1 < argc) ? std::string(argv[++i]) : std::string();
    };
    if (k == "--seed") a.seed = std::strtoull(next().c_str(), nullptr, 10);
    else if (k == "--ticks") a.ticks = std::atoi(next().c_str());
    else if (k == "--bodies") a.bodies = std::atoi(next().c_str());
    else if (k == "--mode") a.mode = next();
    else if (k == "--out") a.out = next();
  }
  return a;
}

std::string json_escape(const std::string& s) {
  std::string out;
  for (char c : s) {
    if (c == '"' || c == '\\') { out += '\\'; out += c; }
    else out += c;
  }
  return out;
}

// ---------------------------------------------------------------------------
// Phase 1: world models in the loop — divergence report
// ---------------------------------------------------------------------------
int run_phase1(const Args& args) {
  ow::Phase1Config cfg;
  cfg.seed = args.seed;
  cfg.ticks = args.ticks;
  cfg.n_bodies = args.bodies;

  std::cout << "omniworld_loop: Phase 1 — world models in the loop\n";
  std::cout << "  seed=" << cfg.seed << " bodies=" << cfg.n_bodies
            << " ticks=" << cfg.ticks << "\n";

  const ow::Phase1Result r = ow::run_world_model_eval(cfg);

  std::cout << "\n== Rollout ==\n";
  std::cout << "  rollout_digest : " << std::hex << r.rollout_digest
            << std::dec << "\n";
  std::cout << "\n== Prediction-vs-truth divergence ==\n";
  std::cout << "  model            n_pred   mean_div    max_div   fit_s   fit_metric   pred_p50_us\n";
  for (const auto& m : r.models) {
    std::cout << "  " << std::left << std::setw(16) << m.model << std::right
              << std::setw(7) << m.n_predictions << std::fixed
              << std::setprecision(6)
              << std::setw(11) << m.mean_divergence
              << std::setw(11) << m.max_divergence
              << "  " << std::setprecision(3) << m.fit_s
              << "  " << std::setw(10) << std::setprecision(6) << m.fit_metric
              << "  " << std::setw(9) << std::setprecision(2)
              << m.predict_p50_us << "\n";
  }
  const ow::ModelEval* replay = nullptr;
  const ow::ModelEval* mlp = nullptr;
  const ow::ModelEval* zero = nullptr;
  for (const auto& m : r.models) {
    if (m.model == "replay") replay = &m;
    if (m.model == "mlp_dynamics") mlp = &m;
    if (m.model == "zero_delta") zero = &m;
  }
  std::cout << "\n== Sanity ordering (oracle/baselines) ==\n";
  if (replay)
    std::cout << "  replay mean divergence == 0 : "
              << (replay->mean_divergence == 0.0 ? "OK" : "BUG") << " ("
              << replay->mean_divergence << ")\n";
  if (replay && zero && mlp) {
    std::cout << "  zero_delta is the floor     : "
              << (zero->mean_divergence > mlp->mean_divergence ? "OK" : "WARN")
              << " (zero=" << zero->mean_divergence
              << " mlp=" << mlp->mean_divergence << ")\n";
  }

  if (!args.out.empty()) {
    const bool rollout_ok = ow::write_rollout_text(
        ow::record_rollout(cfg.seed, cfg.n_bodies, cfg.ticks),
        args.out + "/rollout.txt");
    std::ostringstream js;
    js << "{\n";
    js << "  \"seed\": " << r.seed << ", \"bodies\": " << r.n_bodies
       << ", \"ticks\": " << r.ticks << ",\n";
    js << "  \"rollout_digest\": \"" << std::hex << r.rollout_digest
       << std::dec << "\",\n";
    js << "  \"models\": [\n";
    for (std::size_t i = 0; i < r.models.size(); ++i) {
      const auto& m = r.models[i];
      js << "    {\"model\": \"" << json_escape(m.model)
         << "\", \"n_predictions\": " << m.n_predictions
         << ", \"mean_divergence\": " << m.mean_divergence
         << ", \"max_divergence\": " << m.max_divergence
         << ", \"fit_s\": " << m.fit_s << ", \"fit_metric\": " << m.fit_metric
         << ", \"predict_p50_us\": " << m.predict_p50_us
         << ", \"predict_p95_us\": " << m.predict_p95_us << "}";
      js << (i + 1 < r.models.size() ? "," : "") << "\n";
    }
    js << "  ]\n}\n";
    const std::string path = args.out + "/phase1-report.json";
    std::ofstream f(path, std::ios::binary);
    f << js.str();
    std::cout << "\n  report written: " << path
              << (rollout_ok ? " (+ rollout.txt)" : "") << "\n";
  }
  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  const Args args = parse_args(argc, argv);

  if (args.mode == "phase1") return run_phase1(args);

  ow::LoopConfig cfg;
  cfg.seed = args.seed;
  cfg.ticks = args.ticks;
  cfg.n_bodies = args.bodies;

  std::cout << "omniworld_loop: Grounding Loop v0\n";
  std::cout << "  seed=" << cfg.seed << " bodies=" << cfg.n_bodies
            << " ticks=" << cfg.ticks << "\n";

  const ow::LoopResult r = ow::run_grounding_loop(cfg);

  std::cout << "\n== Loop ==" << std::fixed << std::setprecision(3);
  std::cout << "\n  end_to_end_hz      : " << r.loop_hz << "\n";
  std::cout << "  total_wall_s       : " << r.total_wall_s << "\n";
  std::cout << "  final_state_digest : " << std::hex << r.final_state_digest
            << std::dec << "\n";
  std::cout << "  final_image_digest : " << std::hex << r.final_image_digest
            << std::dec << "\n";
  std::cout << "  adapters           : ";
  for (const auto& n : r.adapters_used) std::cout << n << " ";
  std::cout << "\n";

  std::cout << "\n== Per-stage latency (us) ==\n";
  std::cout << "  stage                         p50        p95\n";
  for (const auto& [stage, samples] : r.stage_us) {
    std::cout << "  " << std::left << std::setw(28) << stage << std::right
              << "  " << std::setw(9) << percentile(samples, 0.50)
              << "  " << std::setw(9) << percentile(samples, 0.95) << "\n";
  }

  std::cout << "\n== Coherence metrics per producer ==\n";
  std::cout << "  producer                props   applied  id_stab  err_mean  hc_wrong_rate  ece\n";
  for (const auto& m : r.producers) {
    std::cout << "  " << std::left << std::setw(24) << m.producer
              << std::right << std::setw(6) << m.n_proposals
              << std::setw(9) << m.n_applied
              << "  " << std::setw(7) << std::setprecision(4)
              << m.identity_stability()
              << "  " << std::setw(8) << m.mean_transform_error()
              << "  " << std::setw(13) << m.high_conf_wrong_rate()
              << "  " << std::setw(7) << m.ece() << "\n";
  }

  // ---- Gate 1 (PLAN.md section 6) ----
  const double A = r.loop_hz;
  const bool gate_a = A >= 30.0;

  bool gate_b = r.adapters_used.size() >= 3;
  for (const auto& n : r.adapters_used) {
    if (n != "blob_cc" && n != "blob_grid" && n != "blob_cc_miscalibrated") {
      gate_b = false;  // unknown adapter => registration path changed
    }
  }

  const ow::ProducerMetrics* cc = nullptr;
  const ow::ProducerMetrics* bad = nullptr;
  for (const auto& m : r.producers) {
    if (m.producer == "blob_cc") cc = &m;
    if (m.producer == "blob_cc_miscalibrated") bad = &m;
  }
  // Gate 1c: the coherence metrics must flag the deliberately miscalibrated
  // model. Metric = ECE separation (standard calibration error catches the
  // inverted-confidence failure mode that a high-conf-wrong rate misses).
  const double discrim = (cc && bad) ? bad->ece() - cc->ece() : -1.0;
  const bool gate_c = discrim >= 0.30;

  std::cout << "\n== Gate 1 ==\n";
  std::cout << "  (a) end_to_end >= 30Hz on this machine      : "
            << (gate_a ? "PASS" : "FAIL") << "  (hz=" << A << ")\n";
  std::cout << "  (b) all models attach adapter-only (registry): "
            << (gate_b ? "PASS" : "FAIL") << "  ("
            << r.adapters_used.size() << " adapters, loop has no per-model code)\n";
  std::cout << "  (c) metrics flag the miscalibrated model    : "
            << (gate_c ? "PASS" : "FAIL") << "  (ece_separation=" << discrim
            << ")\n";
  const bool gate1 = gate_a && gate_b && gate_c;
  std::cout << "  GATE1 VERDICT: " << (gate1 ? "PASS" : "FAIL") << "\n";

  if (!args.out.empty()) {
    std::ostringstream js;
    js << "{\n";
    js << "  \"seed\": " << r.seed << ", \"bodies\": " << r.n_bodies
       << ", \"ticks\": " << r.ticks << ",\n";
    js << "  \"end_to_end_hz\": " << r.loop_hz
       << ", \"total_wall_s\": " << r.total_wall_s << ",\n";
    js << "  \"final_state_digest\": \"" << std::hex << r.final_state_digest
       << std::dec << "\", \"final_image_digest\": \"" << std::hex
       << r.final_image_digest << std::dec << "\",\n";
    js << "  \"stages\": {\n";
    bool first = true;
    for (const auto& [stage, samples] : r.stage_us) {
      if (!first) js << ",\n";
      first = false;
      js << "    \"" << json_escape(stage)
         << "\": {\"p50_us\": " << percentile(samples, 0.50)
         << ", \"p95_us\": " << percentile(samples, 0.95) << "}";
    }
    js << "\n  },\n  \"producers\": [\n";
    for (std::size_t i = 0; i < r.producers.size(); ++i) {
      const auto& m = r.producers[i];
      js << "    {\"producer\": \"" << json_escape(m.producer)
         << "\", \"n_proposals\": " << m.n_proposals
         << ", \"n_applied\": " << m.n_applied
         << ", \"identity_stability\": " << m.identity_stability()
         << ", \"mean_transform_error\": " << m.mean_transform_error()
         << ", \"high_conf_wrong_rate\": " << m.high_conf_wrong_rate()
         << ", \"ece\": " << m.ece() << "}";
      js << (i + 1 < r.producers.size() ? "," : "") << "\n";
    }
    js << "  ],\n";
    js << "  \"gate1\": {\"a_hz_pass\": " << (gate_a ? "true" : "false")
       << ", \"b_adapter_only\": " << (gate_b ? "true" : "false")
       << ", \"c_discrimination\": " << discrim
       << ", \"verdict\": \"" << (gate1 ? "PASS" : "FAIL") << "\"}\n";
    js << "}\n";
    const std::string path = args.out + "/grounding-loop-report.json";
    std::ofstream f(path, std::ios::binary);
    f << js.str();
    std::cout << "\n  report written: " << path << "\n";
  }
  return gate1 ? 0 : 2;
}
