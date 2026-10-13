# OmniWorld Engine

[中文说明 / Chinese README](README.zh-CN.md)

**OmniWorld is a GPU-native world engine (C++20): moving video decode, AI inference, observation rendering, and world-state updates through GPU systems with the least practical copies, synchronization, and latency.** Perception models, world models, simulators, renderers, and agents all share one measurable "living world state" (identity / time / ownership / confidence contract). The evaluation harness is the engine's first application (ADR-0008).

## Document map (where to start)

| Step | Document | Role | After reading you know |
|---|---|---|---|
| ① 5 min | This README | What the project is + where everything lives | The one-liner + navigation |
| ② 15 min | [`PLAN.md`](PLAN.md) | **The one must-read**: identity / goal function / milestone roadmap & Gate system / operating rules | What we're building and the rules |
| ③ 1 min | [`CURRENT.md`](CURRENT.md) | The single current focus + exit criteria | What to open today |
| ④ 20 min | [`docs/design/00-总体设计.md`](docs/design/00-总体设计.md) | **Current authoritative design (frozen)**: World Contract semantics + overall architecture + phase plan | How the contract was decided, what the system looks like |
| ⑤ as needed | [`docs/design/`](docs/design/) stage folders | Stage designs (L1) / slice detailed designs (L2) / [`problem-log.md`](docs/design/problem-log.md) | How a given piece is designed, what broke |
| ⑥ when working | [`.scratch/stage-a-core/`](.scratch/stage-a-core/) | **Task tickets**: spec + slice tickets 01—06 (current active feature) | Which task is live, what its acceptance criteria are |
| ⑦ as needed | [`docs/adr/`](docs/adr/) | The "why" of every major decision: 0008 direction reopen / 0009 dual mode / 0010 blank-slate rebuild / 0011 rear-mirror | Decision history |
| ⑧ as needed | [`GLOSSARY.md`](GLOSSARY.md) · [`benchmarks/regression/`](benchmarks/regression/) | Glossary · the only evidence for performance claims | Precise definitions / where numbers come from |

## Current state (2026-10-13)

- **Blank-slate co-designed rebuild in progress** (ADR-0010): the old implementation is abandoned (see `git tag pre-rebuild`); the rebuild follows a design-first process — L0 overall design is frozen (contract: 9 semantics + architecture + phase plan); stage A high-level design (L1) is drafted, awaiting owner approval.
- The only buildable code right now is `runtime_probe` (standalone hardware probe); the engine returns slice by slice with stage A.
- Pre-rebuild baseline (alignment target): Gate 1 — 1106.7 Hz / adapter integration with zero core changes / ECE separation 0.9617.
- Dev process = the AI Hero main flow: `grill-with-docs → to-spec → to-tickets → implement+tdd → code-review → retro` (skill mapping in ADR-0010).

## Build & run

```bash
build.bat --test                # Windows: the only reliable entry (currently = runtime_probe smoke test 1/1)
build\runtime_probe.exe --json  # hardware capability probe (CPU / CUDA / Vulkan)
```

`omniworld_loop.exe` (the grounding loop) returns with the stage A slices.

## Rules

- **Design first**: no code slice starts without an owner-approved detailed design (L2).
- Facts vs claims; no performance claims without benchmark data; direction/contract-level decisions land as ADRs.
- No new documents before code runs; new ideas go to the research queue, never into a sprint.
