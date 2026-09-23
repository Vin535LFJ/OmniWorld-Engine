# ADR-0009: nvdec-nvenc

## Status

Accepted for planning; implementation remains gated by benchmarks.

## Context

OmniWorld Engine must stay focused on GPU-native real-time dataflow rather than becoming a broad game engine, robotics stack, simulator, or model-training platform.

## Decision

Use this decision to preserve the core boundary: Engine Core owns runtime abstractions, memory/resource lifetime, scheduling, synchronization, metrics, world-state contracts, and minimal rendering/inference/media integration points. External ecosystems remain adapters.

## Consequences

- Positive: scope remains tractable for a personal/small-team project.
- Positive: benchmark results can validate or reject each decision.
- Negative: some integrations may be less convenient than adopting a large framework wholesale.

## Review gate

Revisit when an MVP benchmark proves the decision invalid, too expensive, or unnecessary.
