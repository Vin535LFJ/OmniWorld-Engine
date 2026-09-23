# Research Queue

Effective 2026-09-21 (ADR-0007). Research does not change the roadmap by itself. A topic must produce a sourced research note (and where applicable code/experiment/benchmark evidence) before it can become architecture. Routing: Research → Exploration → Spike/PoC → Benchmark → Architecture Review → ADR → Core.

| Topic | Priority | Track | Status | Why | When / trigger |
| --- | --- | --- | --- | --- | --- |
| WorldFoundry & cosmos-evaluator differential | High | Intelligence | Research | Closest open landmarks (WorldFoundry: shared train/eval infra, 2026-07; cosmos-evaluator: open Apache-2.0 video-scoring). Positioning claim (state-centric real-time loop vs model-centric batch pipelines) must survive scrutiny. | Phase 0–1; output = research note + ADR attachment |
| Action-vocabulary mapping (WorldMark / WBench) | High | Intelligence | Research | WorldMark uses per-model adapters over a WASD-style control vocabulary ("no shared protocol has been established"); Phase-2 interop needs our Action contract to map onto it. | Phase 0–1 reading; feeds Phase-1 Action contract |
| Determinism pitfalls for seeded kinematics | High | Engineering | Research | Week-2 exit requires bit-identical replay: float stability, math library variance, Windows/Linux drift. | Week 2 (in-flight) |
| ONNX Runtime C++ integration on Windows | High | Engineering | Research | Week-3 perception adapters: session setup, CUDA EP availability on the 4060 8 GB, input/output tensor contracts. | Week 2–3 (in-flight) |
| Third-party open interactive world models runnable on 8 GB VRAM | Medium | Intelligence | Research | Phase-1/2 evaluation subjects (MineWorld / Matrix-Game class). Catalog VRAM, action interface, license. | Phase 1 |
| Conflict-resolution policy family | Medium | Intelligence | Research | timestamp-wins vs confidence-weighted vs ownership arbitration (DDS ownership / ROS 2 practice as references). | Phase 3 entry: Phase-0/1 conflict data exists |
| Minimal uncertainty propagation | Medium | Intelligence | Research | Calibration of proposal confidence; when/how to propagate beyond a scalar. | Phase 3 |
| World-state serving papers (2026-07: exact-state serving, world-state registers) | Low | Intelligence | Watch | Serving-side academic work converging on the same gap; track for convergent ideas and citations. | Ongoing; note only |
| OpenUSD one-way export | Low | Intelligence | Research | Future interop with the USD ecosystem; explicitly not compliance. | Post-Gate-2 at the earliest |
| CUDA Graph scheduling overhead | Low | Engineering | Exploration | May reduce runtime scheduling overhead once the loop's scheduling is measured. | After Phase-0 loop benchmarks show scheduling cost |

Retired items (superseded by ADR-0007): TensorRT dynamic shapes (returns when a phase needs it), CV-CUDA (same), Vulkan Video (same), NITROS (Backlog), CARLA/Isaac adapter boundary research (Backlog), Brain-provider prototype (Backlog). Full landscape rationale: `2026-09-21-direction-decision.md`.
