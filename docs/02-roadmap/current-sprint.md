# Current Sprint

## Current Phase

Phase 1 — GPU Runtime Foundation

## Current Goal

Build one runnable OmniWorld Runtime Probe before expanding into video, AI, ROS 2, simulation, or VLA work.

## This Week

Done 2026-09-18 (all on Windows 11; see `benchmarks/regression/2026-09-18-runtime-probe-baseline.md`):

1. [x] Create a minimal C++20/CMake/Ninja repository skeleton.
2. [x] Add a `runtime_probe` executable target.
3. [x] Implement CPU and OS/runtime configuration reporting.
4. [x] Implement CUDA device capability probing: device name, compute capability, driver/runtime version, memory, stream/event smoke timing (64 MiB H2D/D2H p50: ~12.7 / ~13.1 GiB/s).
5. [x] Implement Vulkan physical-device probing: API version, driver version, device type, queue families, memory heaps, validation-layer availability (loader loaded dynamically; no SDK required).
6. [x] Save one sample run output and note missing environment dependencies (artifacts under `benchmarks/regression/artifacts/2026-09-18-runtime-probe-baseline/`).

## Next Week

1. Add structured logging and consistent error/result types.
2. Add metrics helpers for wall-clock, CUDA event timing, and Vulkan timestamp-query readiness.
3. Add JSON or machine-readable probe output. (done early 2026-09-18: `--json` mode exists; keep hardening it)
4. Add a tiny benchmark harness for repeated probe and timing runs (p50/p95/p99 over N invocations).
5. Decide whether the environment is ready for Phase 2 video experiments. Blocker: current machine is Windows 11, not the Linux target from ADR-0003/ADR-0008; Linux CPU-probe branch is written but untested.

## Environment Deviation (recorded)

- Target OS is Linux; development currently happens on Windows 11 (build 26200) with CUDA 12.6 and the system Vulkan loader. No Vulkan SDK or TensorRT installed.

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
