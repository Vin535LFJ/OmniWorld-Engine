# OmniWorld Engine

[中文说明 / Chinese README](README.zh-CN.md)

**OmniWorld is a GPU-native world engine (C++20): moving video decode, AI inference, observation rendering, and world-state updates through GPU systems with the least practical copies, synchronization, and latency.** Perception models, world models, simulators, renderers, and agents all share one measurable "living world state" (identity / time / ownership / confidence contract). The evaluation harness is the engine's first application (ADR-0008).

## Documentation

Three core documents + a support layer:

| File | Role |
|---|---|
| [`PLAN.md`](PLAN.md) | **The one must-read**: identity / goal function / milestone roadmap & Gate system / World Contract / licensing red lines |
| [`CURRENT.md`](CURRENT.md) | The single current focus + exit criteria (now = Phase 2 "AI in the loop") |
| [`GLOSSARY.md`](GLOSSARY.md), `docs/adr/`, `docs/17-research/`, `docs/agents/` | Support layer: glossary, decision records, evidence, conventions |

Historical documents live in `docs/99-archive/` — reference only, do not read through.

## Current state

- Phase 0 + Phase 1 complete (2026-09-28): Gate 1 passed all three criteria — closed loop at 1106.7 Hz, second-model integration with zero core changes, ECE discriminability separation 0.9617; five world models ran through one contract with five-level divergence separation.
- Direction reopened and finalized 2026-10-10 (ADR-0008: GPU-native world engine as the identity).
- Next: **Phase 2 "AI in the loop" — swapping in real models via ONNX Runtime** (see [`CURRENT.md`](CURRENT.md)).

## Build & run

```bash
build.bat --test                # Windows: the only reliable entry (configure + build + ctest)
build\omniworld_loop.exe        # Grounding Loop v0 + Gate 1 (reference mode, byte-exact)
build\omniworld_loop.exe --mode phase1   # world-model divergence evaluation
```

Requirements: MSVC + CMake (`build.bat` configures the environment). Reference mode is dependency-free and byte-reproducible for the same seed; performance mode is tolerance-reproducible (ADR-0009).

## Rules

- Facts vs claims: every technical assertion must trace to repository evidence or external primary sources; no performance claims without benchmark data.
- No new documents before code runs; new ideas go to the research queue, never into a sprint; direction/contract-level decisions land as ADRs.

## Language

`PLAN.md` / `CURRENT.md` are Chinese-first (terms keep their English originals); research notes are in Chinese.
