# Design Principles

## GPU First
Why: The project studies real-time GPU dataflow. Impact: data formats, queues, and ownership are designed around GPU residency. Constraint: CPU paths are baselines/fallbacks, not the primary architecture.

## Zero / Low Copy
Why: Copies dominate latency and bandwidth at video/inference/rendering scales. Impact: every edge in the pipeline must declare copy class. Constraint: never claim zero-copy without benchmark evidence.

## Async by Default
Why: Decode, compute, inference, render, and encode can overlap. Impact: APIs should accept queues/streams and timestamps. Constraint: blocking calls require explicit documentation.

## Explicit Synchronization
Why: Correct GPU interop requires visible ordering. Impact: fences, semaphores, events, and barriers are first-class design objects. Constraint: no hidden global synchronization in core paths.

## Data-Oriented Design
Why: Runtime performance depends on memory layout and batch/stream behavior. Impact: data contracts favor contiguous buffers, stable lifetimes, and clear ownership. Constraint: avoid object-heavy abstractions in hot paths.

## Modular Runtime
Why: A small team needs replaceable components. Impact: media, inference, renderer, world, and adapters are separate modules. Constraint: each module must be mockable.

## Backend Abstraction
Why: Some backends will change. Impact: isolate CUDA/Vulkan/TensorRT/FFmpeg details behind thin contracts. Constraint: abstraction must not hide performance-critical behavior.

## Model Agnostic
Why: LLM/VLM/VLA providers evolve quickly. Impact: Brain consumes observations and returns actions through contracts. Constraint: no provider-specific type in Engine Core.

## Renderer Independent
Why: The project is runtime-first, not renderer-first. Impact: Vulkan renderer is an observation/display module. Constraint: no editor, scripting, or full asset pipeline in MVP.

## Simulation Independent
Why: CARLA, Isaac, PhysX, and other simulators are large ecosystems. Impact: simulation enters through adapters. Constraint: do not clone simulators.

## Observable / Profilable
Why: The project must prove its engineering claims. Impact: metrics and trace hooks are core requirements. Constraint: milestones without measurements are incomplete.

## Benchmark Driven
Why: Performance claims are otherwise marketing. Impact: every phase has benchmark and exit criteria. Constraint: unsupported metrics remain hypotheses.
