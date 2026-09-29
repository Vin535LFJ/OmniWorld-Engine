// omniworld_loop — Grounding Loop v0 runner: executes the closed loop,
// prints the coherence report, and evaluates Gate 1 (PLAN.md section 6).
#include "grounding_loop/loop.hpp"

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
  std::string out;  // optional artifacts dir for the json report
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

}  // namespace

int main(int argc, char** argv) {
  const Args args = parse_args(argc, argv);

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
