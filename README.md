# OmniWorld Engine

[中文说明 / Chinese README](README.zh-CN.md)

📖 **Lost track of the project? Start with the owner briefing series → [`docs/19-owner-briefing/`](docs/19-owner-briefing/)**

**OmniWorld Engine is the open runtime that keeps heterogeneous AI systems — perception models, world models, simulators, renderers, and agents — grounded in one evolving, measurable world state.**

Interactive world models are being asked to become "world simulators" and "environments for training agents", yet no open system lets them share one live world state with a ground-truth simulator and an agent under explicit contracts (identity, time, ownership, confidence). That capability is either locked inside NVIDIA's proprietary stack (Mega Omniverse Blueprint / `ovstage`) or hand-rewritten inside every interactive world-model evaluation project. OmniWorld is the open, neutral runtime for that loop — grounding measured, not assumed.

中文：**OmniWorld Engine 是一个开放的实时 runtime，让感知模型、世界模型、仿真器、渲染器与智能体在同一个可测量的、持续演化的世界状态上保持一致（identity / time / ownership / confidence 契约）。它首个立足点是交互式世界模型的闭环评测 harness。**

## 🚧 Current Development Focus

Phase: **Phase 0 — Grounding Loop v0** (first 30 days; direction decision 2026-09-21, see [`ADR-0007`](docs/01-architecture/decisions/ADR-0007-project-direction.md))

Current Goal: prove the Minimum Viable Thesis on an RTX 4060 8 GB — a deterministic synthetic world, two local perception adapters, ownership adjudication, and coherence metrics, in a closed loop at interactive rate, with three explicit falsification criteria (Gate 1 in [`current-sprint.md`](docs/02-roadmap/current-sprint.md)).

Status: planning is **closed and frozen** (2026-09-21). `runtime_probe` (C++20 CPU/CUDA/Vulkan hardware probe) builds and is committed; the first regression baseline exists at `benchmarks/regression/2026-09-18-runtime-probe-baseline.md`.

## Why now

- 2025–2026 interactive world-model evaluation fragmented (WBench, WorldMark, WorldArena, KineBench, ...): every team re-implements action adapters, state schemas, and simulator couplings. WorldMark: *"no shared protocol has been established."*
- The industrial version of the "world runtime" is proprietary and NVIDIA-ecosystem-only; AOUSD's ratified spec covers scene description, not runtime semantics.
- No open player ships a runtime layer: Genie 3 has no API; World Labs ships world generation + persistent assets, not a live stateful runtime.

Full evidence: [`docs/17-research/2026-09-21-direction-decision.md`](docs/17-research/2026-09-21-direction-decision.md).

## What it is

- **Core:** World Contract v0 (`docs/01-architecture/world-state.md`), the loop runtime (versioned canonical state, frame-budget accounting), grounding metrics (identity stability, transform error vs truth, temporal coherence), a deterministic truth world.
- **Adapters:** local ONNX perception models, replay data, a tiny dynamics model (Phase 1), external physics (Phase 3) — plus an optional World API scene adapter, never in the benchmark mainline.
- **Not:** an industrial digital-twin platform, a world-model training project, a game engine/editor/renderer replacement, or a "connect everything" platform. The rule: a feature enters Core only if we can name a metric only OmniWorld can measure.

## Documentation

- Daily entry: [`CURRENT.md`](CURRENT.md)
- This sprint (30 days): [`docs/02-roadmap/current-sprint.md`](docs/02-roadmap/current-sprint.md)
- Canonical roadmap + gates: [`docs/02-roadmap/master-roadmap.md`](docs/02-roadmap/master-roadmap.md)
- World Contract v0: [`docs/01-architecture/world-state.md`](docs/01-architecture/world-state.md)
- Decisions: [`docs/01-architecture/decisions/`](docs/01-architecture/decisions/)
- Research queue: [`docs/17-research/research-queue.md`](docs/17-research/research-queue.md); legacy planning material: `docs/99-archive/`
- Benchmarks: [`benchmarks/`](benchmarks/)

## How to Build and Run

```bash
cmake -B build -G Ninja && cmake --build build
./build/runtime_probe            # hardware probe (CPU/CUDA/Vulkan capabilities)
./build/runtime_probe --json     # machine-readable output
ctest --test-dir build           # smoke test
```

Requires: C++20 compiler, CMake + Ninja; CUDA Toolkit and the system Vulkan loader are auto-detected (optional). The Grounding Loop targets Phase 0 (see sprint).

## Contributing

Keep facts, plans, hypotheses, experiments, and verified results separate. Every technical claim must trace to project evidence or an external primary source. The planning phase is closed: ideas enter via the research queue, not by editing the roadmap.

## License

License status should be finalized before accepting external contributions.
