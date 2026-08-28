# Current Sprint

## Current Phase

Phase 1 — GPU Runtime Foundation

## Current Goal

Build one runnable OmniWorld Runtime Probe before expanding into video, AI, ROS 2, simulation, or VLA work.

## This Week

1. Create a minimal C++20/CMake/Ninja repository skeleton.
2. Add a `runtime_probe` executable target.
3. Implement CPU and OS/runtime configuration reporting.
4. Implement CUDA device capability probing: device name, compute capability, driver/runtime version, memory, stream/event smoke timing.
5. Implement Vulkan physical-device probing: API version, driver version, device type, queue families, memory heaps, validation-layer availability.
6. Save one sample run output and note missing environment dependencies.

## Next Week

1. Add structured logging and consistent error/result types.
2. Add metrics helpers for wall-clock, CUDA event timing, and Vulkan timestamp-query readiness.
3. Add JSON or machine-readable probe output.
4. Add a tiny benchmark harness for repeated probe and timing runs.
5. Decide whether the environment is ready for Phase 2 video experiments.

## Current Learning

- C++20: targets, RAII, error/result types, and simple CLI structure.
- CMake/Ninja: reproducible configure/build/test workflow.
- CUDA: device model, memory properties, streams, events, synchronization, and timing.
- Vulkan: instance creation, physical-device enumeration, queues, memory heaps, validation layers.
- Linux: GPU driver discovery and reproducible environment notes.

## Current Coding

- `runtime_probe` executable.
- CUDA capability probe module.
- Vulkan capability probe module.
- Logging and metrics module.
- First benchmark/check script or documented command.

## Current Benchmark

- Probe startup latency.
- CUDA event timing overhead.
- Vulkan instance/device enumeration latency.
- Reported VRAM and driver/runtime versions.
- Build and run reproducibility on the current Linux machine.

## Definition of Done

Phase 1 is done when `runtime_probe` builds from a clean checkout and outputs CPU, GPU, CUDA, Vulkan, VRAM, driver, runtime configuration, and basic timing metrics with one documented repeatable command.
