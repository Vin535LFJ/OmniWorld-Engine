# Current Sprint — Grounding Loop v0 (first 30 days)

Phase 0 of [`master-roadmap.md`](master-roadmap.md). One milestone per week; weekly review follows `weekly-review.md`. Direction and falsification criteria: [`../01-architecture/decisions/ADR-0007-project-direction.md`](../01-architecture/decisions/ADR-0007-project-direction.md).

## Week 1 — Direction lands in the repo + preservation (done 2026-09-21)

- [x] Preservation commit (`bb05d26`): previously-untracked `src/` probe code, `CMakeLists.txt`, `.gitignore`, benchmark baseline, and both root research reports committed; `.zcode/` session files excluded from VCS.
- [x] Direction documents: vision.md rewrite, ADR-0007, world-state.md → World Contract v0, master-roadmap reset, this sprint reset; project-status / not-now / BACKLOG / CURRENT / research-queue updated; README (EN/ZH) repositioned.
- [x] Legacy planning archived (not deleted) under `docs/99-archive/2026-09-planning-era/`: stub `projects/`, duplicate roadmap documents, legacy docs-root files, old ADR set, old architecture stubs.

Verify: every new document passes the Contract Gate checklist (Input/Output/Ownership/Lifetime/Synchronization/Timestamp/Error model/Performance expectations where applicable); internal links resolve; `git ls-files` contains all source code.

## Week 2 — World State + truth world + observation

1. Implement WorldState v0 per `../01-architecture/world-state.md`: entity registry (runtime-minted ids), ticks, state versioning, world-transition logging.
2. Deterministic synthetic truth world: N rigid bodies, seeded, kinematic integration. Determinism test: same seed → bit-identical state/observation sequences.
3. Offscreen observation generator (low-resolution software raster is sufficient; visual fidelity is not the claim).
4. Memory-contract compliance for the frame buffer (fields from `../01-architecture/memory.md`): backend/device/format/dimensions/lifetime/sync state/timestamp declared.

Verify: determinism test passes in CI (`ctest`); observation production ≥ 60 Hz headroom measured and recorded.

## Week 3 — Perception adapters ×2 + loop closure

1. Perception Adapter Contract frozen (same Gate fields as Week 1 checklist) covering: input observation metadata, output proposals (`target`, fields, `confidence`, `tick_observed`).
2. Two DIFFERENT local ONNX perception models (e.g., two lightweight detector/segmenter classes) wired through the contract.
3. Ownership adjudication: truth owns kinematics; perception writes belief-layer proposals; runtime records applied/rejected with reasons.
4. First coherence metrics: identity stability (association accuracy), transform error (belief vs truth), confidence-calibration behavior.

Verify: loop closes (State → Observation → Perception → Proposals → Adjudication → Metrics Recording). Coherence metrics distinguish the normal model from a deliberately miscalibrated variant (Gate 1c evidence).

## Week 4 — Benchmark v0 + Gate 1

1. Full benchmark run: per-stage p50/p95 latency decomposition, end-to-end loop rate, adapter-invariance test (swap the second perception model with zero core edits — Gate 1b evidence).
2. Regression report written to `benchmarks/regression/` with the full environment metadata discipline of the 2026-09-18 baseline.
3. Gate 1 verdict recorded in this file and in the regression report: PASS (proceed to Phase 1) / SHRINK MVP (reduce resolution, drop render stage) / REVISE CONTRACT.

## Environment deviation (recorded)

Development machine: Windows 11 (build 26200), i9-14900HX, RTX 4060 Laptop 8 GB, CUDA 12.6, system Vulkan loader, no Vulkan SDK, no TensorRT yet. Windows-first development is the standing practice per ADR-0007 (superseding strict Linux-first); dual-platform CI is a goal, not a blocker. TensorRT installation is deferred to the first phase whose benchmark actually needs it (perception runs through ONNX Runtime in Phase 0).

## Definition of Done (Phase 0)

Grounding Loop v0 runs end-to-end against the three Gate-1 falsification criteria, with the measurements recorded in a regression report using full benchmark metadata discipline.
