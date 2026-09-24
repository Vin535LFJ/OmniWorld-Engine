# Master Roadmap (canonical)

Single mainline. No parallel phases. Working assumption: solo, ~20 h/week, Windows 11 + RTX 4060 Laptop 8 GB (Windows-first development per ADR-0007).

Thesis / North Star: [`../00-overview/vision.md`](../00-overview/vision.md).
Decision record: [`../01-architecture/decisions/ADR-0007-project-direction.md`](../01-architecture/decisions/ADR-0007-project-direction.md).
Evidence: [`../17-research/2026-09-21-direction-decision.md`](../17-research/2026-09-21-direction-decision.md).
This file supersedes all legacy plan documents (archived in `docs/99-archive/2026-09-planning-era/`).

## Operating rules — when to research, decide, prototype, build, measure

| Stage | Input | Output | Exit criteria |
| --- | --- | --- | --- |
| **Research** | An open question that blocks a phase exit, or a queued Exploration item | Research note with primary sources | The question has a sourced answer, or is proven irrelevant to the current phase |
| **Architecture** | A contract is being frozen (World Contract, Adapter Contract, loop structure) | ADR in `../01-architecture/decisions/` | The ADR names options, trade-offs, and the benchmark that would reverse it |
| **Spike / Prototype** | A named falsifiable technical question | Throwaway code + benchmark note | The question is answered with measured numbers |
| **Implementation** | Frozen contracts | Runtime code + tests | Contract Gate passes (Input/Output/Ownership/Lifetime/Synchronization/Timestamp/Error model/Performance expectations defined); tests pass |
| **Benchmark** | A runnable capability | Regression report (metadata discipline per `docs/16-benchmarks/benchmark-system.md`) | Numbers recorded with full environment metadata; claims traceable to the report |
| **Review** | End of sprint/week | Updated sprint doc + gate verdict | Exit criteria judged against measurements, not feelings |

New ideas never enter the current sprint directly. Path: Research Queue → Backlog → master-roadmap phase → current-sprint → code/experiment/benchmark. After 2026-09-21 the planning phase is closed: sprint documents are for execution, not expansion.

## Phase 0 — Grounding Loop v0 (Weeks 1–4, first 30 days)

|  |  |
| --- | --- |
| Goal | Prove the Minimum Viable Thesis on real hardware: one canonical state, two heterogeneous writers, measurable coherence |
| Capability | WorldState v0 (World Contract v0), deterministic truth world, offscreen observation, two perception adapters, ownership adjudication, coherence metrics |
| Experiment | "One world, two writers": ground truth + two different local ONNX perception models in one closed loop |
| Code | `src/world_state` + `src/truth_world` + `src/perception_onnx` + loop + metrics recorder |
| Benchmark | End-to-end loop rate; per-stage p50/p95 latency decomposition; identity stability; transform error (belief vs truth); calibration-discrimination test |
| Portfolio artifact | The Gate-1 regression report: reproducible closed-loop coherence benchmark on consumer GPU hardware, methodology public |
| Exit (Gate 1) | See Decision Gates below. Detail: [`current-sprint.md`](current-sprint.md) |

## Phase 1 — World-Model Adapter & World Contract v0.1 (Months 2–3)

|  |  |
| --- | --- |
| Goal | Bring the "world model" participant class into the loop — the first contact with what the evaluation community measures |
| Capability | Replay adapter (recorded rollouts as model evidence), tiny self-trained dynamics model (not a foundation model), prediction-vs-truth divergence metric, World Contract v0.1 freeze |
| Code | Replay adapter + dynamics-model adapter + divergence instrumentation |
| Benchmark | Prediction(t+1) divergence vs truth transition; loop rate under replay load |
| Portfolio artifact | First public benchmark note: prediction-vs-truth divergence of a world model inside a shared-state loop |
| Exit | A NEW model joins the loop by writing one adapter file with zero core changes; first publishable benchmark note (benchmarks/regression); WorldFoundry/cosmos-evaluator differential ADR recorded (research-queue item) |

## Phase 2 — Public v0.1 & Evaluation-Community Contact (Months 4–6)

|  |  |
| --- | --- |
| Goal | Become usable outside this repository |
| Capability | 3+ third-party evaluation subjects (open interactive models at MineWorld/Matrix-Game class if runnable, recorded rollouts otherwise), WorldMark/WBench action-vocabulary mapping for the Action contract, public-facing benchmark report, packaging |
| Code | Action-contract conventions + report pipeline polish + docs |
| Benchmark | Public benchmark report v0.1 with reproducible methodology |
| Portfolio artifact | Public harness v0.1 release + benchmark report + first merged PR into an evaluation-ecosystem OSS repo (see reuse-map contribution targets) |
| Exit (Gate 2) | ≥ 1 genuine external interest signal (issue/citation/evaluation-team contact) **OR** door-opener evidence (merged PR in an evaluation-ecosystem repo, collaboration invitation, interview-grade public demo). Fail → shrink to a personal research instrument; stop ALL platform investment |

## Phase 3 — Contract Hardening & Second Truth World (Months 7–9)

|  |  |
| --- | --- |
| Goal | Harden the contract against the failure classes it actually encountered |
| Capability | Conflict-policy family (timestamp-wins, confidence-weighted) as experiment, per-field uncertainty extension, external physics adapter (Jolt/Rapier class), second truth world |
| Benchmark | Reproducibility of coherence metrics across two different worlds |
| Portfolio artifact | Contract-hardening write-up: ownership/conflict handling in a shared world state, backed by measured data |
| Exit | Metrics reproduce across worlds; ROS 2 bridging demand re-evaluated with real evidence |

## Phase 4 — Distribution & Ecosystem Test (Months 10–12)

|  |  |
| --- | --- |
| Goal | Test the ecosystem hypothesis |
| Capability | Clean distribution path, 3+ documented adapter examples, one submittable system/benchmark note |
| Portfolio artifact | Submittable system/benchmark note + a packaged release strangers can run |
| Exit (Gate 3) | ≥ 2 real external uses (an evaluation, an experiment, a course assignment) **OR** sustained door-opener traction (multiple merged PRs / active collaboration with a target team). Fail → freeze expansion; maintain as a personal instrument |

## Decision Gates

| Gate | When | Question | Pass criteria | Fail action |
| --- | --- | --- | --- | --- |
| **Gate 1** | Day 30 | Is the loop technically real? | (a) end-to-end ≥ 30 Hz on RTX 4060 8 GB; (b) second perception model integrates adapter-only (zero core edits); (c) metrics discriminate a deliberately miscalibrated model | Shrink MVP (lower resolution / drop render stage) or revise World Contract before ANY new feature |
| **Gate 2** | Month 6 | Does anyone want this? | ≥ 1 genuine external signal | Downgrade to personal research instrument; stop platform investment |
| **Gate 3** | Month 12 | Is an ecosystem seeding? | ≥ 2 real external uses | Freeze scope; maintain as tool; no further expansion |

Gates are judged on recorded evidence only. A gate cannot be "passed in principle." **Gate 1 is technical and NOT dual-track** — the thesis stands or falls on measurements. Gates 2/3 are dual-track per the success model in `../00-overview/vision.md`.

## Now explicitly NOT built (2026-09-21, validated list)

Vulkan full renderer; foundation-model training; distributed/multi-node runtime; ROS 2 bridge; VLA/LLM/Brain integration; editor; commercial service; OpenUSD full compliance (one-way export stays Exploration); industrial digital-twin use cases (NVIDIA Mega's territory); games/application demos; Ray tracing; mobile/Jetson; FFmpeg/NVDEC media path (returns only when a phase's benchmark needs it).
