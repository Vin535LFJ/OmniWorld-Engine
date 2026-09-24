# Vision

Effective 2026-09-21, superseding the pre-decision positioning (direction decision: [ADR-0007](../01-architecture/decisions/ADR-0007-project-direction.md); full evidence: [direction-decision report](../17-research/2026-09-21-direction-decision.md)).

## Project Thesis — why OmniWorld must exist

Interactive world models are being asked to become "world simulators" and "environments for training agents", yet no open system lets a world model, a ground-truth simulator, a renderer, and an agent share one live world state under explicit contracts. That capability is either locked inside NVIDIA's proprietary stack (Mega Omniverse Blueprint + `ovstage`) or hand-rewritten inside every evaluation project (WBench, WorldMark, WorldArena, KineBench, ... each rebuilds action adapters, state schemas, and simulator couplings). OmniWorld is the open runtime for that loop: one canonical, GPU-resident world state; identity/time/ownership/confidence contracts; any model attaches through a thin adapter; grounding is **measured**, not assumed.

## North Star (5 years, falsifiable at the gates)

When someone builds or evaluates an interactive world model, "plug it into OmniWorld and run closed-loop grounding metrics" is the standard path — what MLPerf is for measurement. OmniWorld is the open runtime that keeps heterogeneous AI systems grounded in the same evolving world. Whether this is achieved is decided by Gates 1/2/3 in the canonical roadmap, not by feature count.

## Core Problem

Cross-model grounding: how perception models, predictive/generative world models, simulators, renderers, and agents stay coherent on one shared, time-evolving world state — entity identity, coordinate frames, time, ownership, uncertainty — inside a measurable real-time frame budget.

## Why Now

- 2025–2026: interactive world-model evaluation exploded, and its fragmentation is acknowledged by its own authors ("no shared protocol has been established" — WorldMark; "evaluation remains fragmented" — WorldArena; "disparate codebases, data pipelines, and evaluation protocols" — stable-worldmodel).
- NVIDIA absorbed the industrial version into a proprietary, NVIDIA-ecosystem-only stack; the open neutral version does not exist.
- AOUSD's ratified OpenUSD Core Spec 1.0 standardizes scene description and file formats only — no runtime synchronization, ownership, or model-facing semantics.
- Since 2026-07 academia began touching the state/serving layer ("exact-state serving", "world-state registers"). The window is open now, and it will not stay open.

## Why OmniWorld (this project, this maintainer)

- The scarce skill combination this direction needs — performance engineering × AI-systems integration — is exactly what the repository has been practicing (zero-copy discipline, benchmark metadata, contract-first governance).
- A solo maintainer can enter through the evaluation wedge without an ecosystem team: Phase 0 runs entirely on an RTX 4060 8 GB with local open models and synthetic worlds.
- Zero head-on collision with NVIDIA (industrial fleets and own-model pipelines are theirs; neutral evaluation infrastructure is unclaimed).

## What OmniWorld Is

- The open, real-time **grounding-loop runtime**: canonical versioned World State + World Contract v0 + closed-loop coherence metrics + thin adapters for perception / world-model / renderer / simulator participants.
- First proving ground: the shared closed-loop harness that interactive world-model evaluation teams currently re-implement by hand.

## What OmniWorld Is NOT

- Not an industrial digital-twin platform (that is NVIDIA Mega's territory).
- Not a world-model or foundation-model training project.
- Not a game engine, editor, physics engine, renderer, or ROS replacement.
- Not a "connect everything" platform: **a feature enters Core only when we can name the metric only OmniWorld can measure.**

## Success Model (two tracks, added 2026-09-24 per owner decision)

**Track A — ecosystem adoption** (the North Star above): evaluation teams and researchers actually use OmniWorld. **Track B — personal craft + door-opener**: the skills and public artifacts produced by the main line (reproducible benchmark reports, a runnable harness, merged PRs into evaluation-ecosystem OSS, engineering write-ups) are themselves a target outcome — the portfolio that opens collaboration with the teams building this frontier (NVIDIA Omniverse/Cosmos/Isaac, World Labs, DeepMind, Hugging Face, robotics-sim teams). Track B never substitutes for technical falsification: Gate 1 still judges the thesis on measurements alone. Gates 2/3 are dual-track: external adoption **or** door-opener evidence counts. Discipline unchanged: nothing enters the main line merely to look impressive.

Thesis → contract → roadmap: [`world-state.md`](../01-architecture/world-state.md) · [`master-roadmap.md`](../02-roadmap/master-roadmap.md) · [`current-sprint.md`](../02-roadmap/current-sprint.md)
