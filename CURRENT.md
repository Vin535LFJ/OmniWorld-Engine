# Current Focus

Current Phase: **Phase 0 — Grounding Loop v0** (first 30 days)
Current Project: Grounding Loop v0 — one canonical world state, two writers, measurable coherence (ADR-0007).
Current Week: **Week 2** (see [`docs/02-roadmap/current-sprint.md`](docs/02-roadmap/current-sprint.md) for the full 30-day plan).

## Learn Now

1. World Contract v0 semantics (`docs/01-architecture/world-state.md`): entity identity, tick domain, ownership, belief vs truth.
2. Deterministic seeded simulation basics: fixed-point/stable arithmetic for kinematic integration, reproducibility discipline.
3. ONNX Runtime C++ API basics for the Week-3 perception adapters (two small detector/segmenter class models).
4. The loop's frame-budget math from the 2026-09-18 baseline (H2D ~12.7 GiB/s) to size observation transfers.

## Build Now (Week 2)

1. `src/world_state`: entity registry (runtime-minted ids), tick/clock domain, state versioning, transition logging.
2. Truth world: N seeded rigid bodies, kinematic integration, deterministic-by-construction.
3. Offscreen Observation generator (software raster, low resolution).
4. Determinism test: same seed → bit-identical state/observation sequences (in `ctest`).
5. Frame-buffer memory metadata compliance (`docs/01-architecture/memory.md` fields).

## Research Now

- Only what unblocks Week 2/3 code: preprocessor-free determinism pitfalls, ONNX Runtime session setup on the dev machine, metric formulas for identity stability.
- Standing open Explorations (bounded): WorldFoundry/cosmos-evaluator differential; WorldMark/WBench action vocabulary. Outputs are research notes, never code in Core.

## Do Not Touch

- No ROS 2, CARLA, Isaac, VLA, World Model training, LLM/agent frameworks, digital human, full renderer, media pipeline, editor, distribution work, or platform evangelism (see `docs/02-roadmap/not-now.md`).
- No architecture re-litigation inside Phase 0: the World Contract v0 is frozen; issues route to an ADR review at Gate 1, not to mid-sprint rewrites.
- No performance claims without a regression-report benchmark.

## This Week's Exit

Determinism test passes in CI; observation production ≥ 60 Hz headroom measured and recorded in a benchmark note.
