# Current Focus


Current Phase: Phase 1 — GPU Runtime Foundation


Current Project: GPU Runtime Foundation

Current Goal: Build the first runnable OmniWorld Runtime Probe that reports CPU, GPU, CUDA, Vulkan, VRAM, driver, and runtime configuration.


## Learn Now

1. C++20 project structure, CMake/Ninja targets, and Linux runtime discovery.
2. CUDA device model, memory properties, Stream, Event, async copy, and synchronization.
3. Vulkan instance, physical device, queue family, memory heap, driver properties, and validation layers.
4. Benchmark basics: warmup, repeated samples, p50/p95/p99, and reproducible logs.

## Build Now

1. `runtime_probe` executable.
2. CPU/OS/runtime configuration probe.
3. CUDA capability and timing probe.
4. Vulkan physical-device and validation-layer probe.
5. Logging, metrics, and one repeatable benchmark/check command.

## Research Now

- Only research what unblocks the Runtime Probe or a bounded Exploration item.
- Use `docs/03-engineering-lifecycle/exploration-rules.md` before starting any Spike / PoC.
- Promote research to Core only through Benchmark → Architecture Review → ADR.

## Engineering Exploration

- CUDA Graph scheduling overhead research for later Phase 1/2 optimization.
- CUDA ↔ Vulkan external memory/semaphore interop Spike planning only; do not integrate before the GPU Frame contract exists.

## Intelligence Exploration

- World State / World Model / Agent / ROS 2 / VLA reading and Tiny Prototypes are allowed within the weekly exploration budget.
- Exploration cannot change Core Architecture or replace the active GPU Runtime work.

## Do Not Touch

- Do not make ROS 2, CARLA, Isaac, VLA, World Model, Agent, or Digital Human part of Core now.
- Do not build a full renderer, full video engine, full game engine, physics engine, editor, mobile runtime, Jetson port, or distributed runtime.
- Do not connect major modules before an Interface / Data Contract exists.
- Do not claim performance improvements without Benchmark evidence.

## This Week

1. Create the C++20/CMake/Ninja project skeleton.
2. Add the `runtime_probe` target.
3. Implement CPU and OS/runtime configuration reporting.
4. Implement CUDA capability probing.
5. Implement Vulkan physical-device probing.
6. Add basic logging, error handling, and metrics timestamps.
7. Record the first probe output under the benchmark workflow.

## Next Milestone

`runtime_probe` builds and runs from a clean checkout with reproducible CPU/GPU/CUDA/Vulkan capability output.

## Definition of Ready

Implementation may proceed when the Runtime Probe problem, inputs, outputs, dependencies, environment assumptions, and benchmark target are clear.

## Definition of Done

Runtime Probe builds and runs locally, prints CPU/GPU/CUDA/Vulkan/VRAM/driver/configuration data, records basic timing metrics, and links code, test/check output, benchmark notes, documentation, and weekly review.

