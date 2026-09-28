# Exploration Rules

Exploration keeps Intelligence and uncertain Engineering topics alive without disrupting the active implementation route.

## Tracks

```text
ACTIVE PROJECT: one Engineering project only
Engineering Exploration: limited GPU/runtime/rendering research
Intelligence Exploration: limited Agent / World Model / ROS 2 / VLA research
```

Default weekly time split:

- 70–80% Active Engineering
- 10–20% Intelligence Exploration
- 10% Review / Documentation / Research

## Exploration May Do

- Read papers, official docs, and selected GitHub repositories.
- Run small API calls.
- Build Tiny Prototypes or Spikes.
- Measure a narrow Benchmark.
- Produce a PoC report.
- Add findings to Research Queue or Knowledge with trust labels.

## Exploration May Not Do

- Directly modify Core Architecture.
- Start a large implementation.
- Create a new foundation system.
- Replace the active project.
- Claim performance improvements without benchmark evidence.
- Treat Hypothesis as Verified Knowledge.

## Promotion to Core

```text
Research
 ↓
Exploration
 ↓
Spike / PoC
 ↓
Benchmark
 ↓
Architecture Review
 ↓
ADR
 ↓
Core
```

A technology enters Core only if the review shows it solves a current roadmap problem better than alternatives and has measurable evidence.

## Architecture Gate

Major choices must follow:

```text
Research → Options → Trade-off → Decision → ADR
```

Use this gate for CUDA vs OpenCL, Vulkan vs OpenGL, ROS 2 Core vs Adapter, LLM API vs Local Model, CARLA vs Isaac, GPU buffer strategy, memory ownership, synchronization, and cross-module contracts.

## Contract Gate

Before connecting major modules, define:

- Input
- Output
- Ownership
- Lifetime
- Synchronization
- Timestamp
- Error Model
- Performance Expectations

No Contract, no large-module integration.
