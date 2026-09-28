# ADR-0007: Project Direction — Evaluation-First Grounding-Loop Runtime

Status: Accepted
Date: 2026-09-21
Trust Label: [Architecture Decision]
Evidence base: [`../../17-research/2026-09-21-direction-decision.md`](../../17-research/2026-09-21-direction-decision.md)

## Context

As of 2026-09-18 the repository held ~10,700 lines of committed planning markdown and exactly one runnable artifact: `runtime_probe` (~1,100 lines of C++20, committed in bb05d26). The committed identity ("GPU-native runtime for connecting everything through a unified world representation") and the two untracked research reports (an argument for a "World Runtime" pivot) conflicted. The maintainer is solo, on Windows 11, RTX 4060 Laptop 8 GB — a hardware reality that rules out running Cosmos-class world models locally (Cosmos 3 Nano 16B lists RTX PRO 6000 / H100-class GPUs).

At the same time, the 2025–2026 interactive world-model ecosystem exploded (Cosmos 3, Genie 3, World Labs Marble/RTFM/Atlas, Decart, Runway GWM) and its **evaluation layer fragmented**: WBench, WorldMark, WorldArena, WorldRoamBench, iWorld-Bench, KineBench, TriWorldBench, stable-worldmodel, and WorldFoundry each re-implement action adapters, state schemas, and simulator couplings. WorldMark's paper states it outright: "no shared protocol has been established." Meanwhile the only closed-loop "perception → world state → simulation → agent" runtime that actually ships — NVIDIA Mega Omniverse Blueprint with the proprietary `ovstage` scene substrate — is NVIDIA-ecosystem-only with no standard interface for third-party world models. AOUSD's ratified Core Spec 1.0 standardizes scene description and file formats only; runtime sync, ownership, and model-facing contracts are out of its scope.

## Problem

Choose the project's identity. The candidates were: (A) stay a GPU runtime, (B) build the full Real-Time World Runtime / World Substrate, (C) build a World Model, (D) none — a narrower direction.

## Options

| Option | Pros | Cons | Risks |
| --- | --- | --- | --- |
| A — GPU Runtime | Feasible solo; existing probe code fits | "Fast plumbing" gives no reason to exist; NVIDIA DALI/NIM/tooling cover plumbing; no demand story | Irrelevance; silent drift |
| B — Full World Substrate | Real white-space claim (no open neutral runtime) | Competes head-on with NVIDIA Mega (proprietary but shipped and backed); platform plays need an ecosystem team; scope explodes before any demand proof | Scope explosion; Mega collision; years without a user |
| C — Foundation World Model | Highest ceiling | Requires data/compute acquire unavailable to a solo lab; user's explicit constraint forbids it | Impossible to finish |
| D — Evaluation-First Grounding-Loop Runtime | Same core technical problem as B (canonical state + identity/time/ownership contracts + real-time loop) attacked through the one demand-verified wedge (evaluation); runnable on an RTX 4060; falsifiable in 30 days | Narrower ambition upfront; positioning vs WorldFoundry must be defended early; benchmark credibility is earned slowly | Ecosystem adoption may never materialize → mitigated by explicit stop-gates |

Dimension detail (feasibility, research/engineering difficulty, differentiation, competitive pressure, long-term potential, ability to start small): direction-decision report §7.

## Decision

Adopt **Option D**: OmniWorld is an open, real-time grounding-loop runtime for interactive world-model systems. Core = World Contract v0 + loop-runtime + grounding metrics + deterministic truth world. Everything model-like (perception, world models), every simulator, every renderer, every agent attaches through thin adapters. Evaluation is the first proving ground: the shared closed-loop harness that benchmark teams currently hand-write per project.

D is not a new direction separate from B: it is B's falsifiable, demand-verified subset. If the harness earns external use (Gate 2/3), the project can grow into the broader substrate with evidence. If it does not, the stop-gates bound the loss.

This ADR also supersedes, **in implementation practice**, the strict Linux-first stance of ADR-0003 (and of archived legacy ADR-0008): development is Windows-first (the actual machine), with dual-platform CI as a goal. "NVIDIA GPU first" is unchanged. It also resolves the recording error from the physics-IQ lineage (see direction-decision §2): legacy claims in the two root research reports remain superseded by this ADR where they conflict.

## Rationale

1. The white space is verified from six directions (industry stack, model landscape, middleware, evaluation ecosystem, prior-art search, demand signals) — see direction-decision §5.
2. Solo feasibility is real: models are adapters, ground truth is deterministic and minimal, and the first loop needs no ML training at all (Week-2 kinematics) and two small local ONNX models (Week 3).
3. Differentiation isolates to semantics no competitor ships openly: ID/time/ownership/confidence contracts plus closed-loop coherence metrics with frame-budget accounting — not code volume.
4. Stop-loss is explicit: Gate 1 (day 30), Gate 2 (month 6), Gate 3 (month 12) in `../../02-roadmap/master-roadmap.md`.

## Consequences

- The GPU runtime becomes the **foundation**, not the identity. Its migration: media (FFmpeg/NVDEC) and inference (TensorRT) work become adapters, not core milestones.
- Roadmap fully reset: `../../02-roadmap/master-roadmap.md` is the single canonical roadmap; legacy plan documents are archived under `docs/99-archive/2026-09-planning-era/`.
- `world-state.md` is rewritten as World Contract v0 (usage-driven, additive-only).
- All planning is frozen after this ADR lands: new ideas route through the Research Queue → Backlog path only.
- Success is measured by gates and external signals, never by doc or feature count.

## Benchmark / Evidence

- Direction evidence: `../../17-research/2026-09-21-direction-decision.md` (all load-bearing claims carry primary-source URLs; corrections to R1/R2 cataloged in §2).
- Feasibility evidence: `runtime_probe` baseline `benchmarks/regression/2026-09-18-runtime-probe-baseline.md` (H2D ~12.7 GiB/s on the actual dev machine informs frame-budget math).
- Phase-0 evidence requirement (Gate 1): loop ≥ 30 Hz end-to-end on RTX 4060 8 GB; second perception model enters with adapter-only changes; coherence metrics discriminate a deliberately miscalibrated model from a normal one.

## Follow-up

1. WorldFoundry / cosmos-evaluator differential comparison — due in Research Queue (High), Phase 0–1.
2. Action-vocabulary mapping to WorldMark / WBench adapter conventions — Phase 1.
3. Execute the 30-day sprint in `../../02-roadmap/current-sprint.md`; Gate 1 verdict at day 30.
