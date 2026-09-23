# Weekly Learning Plan

Learning must serve current code. Do not study a topic unless it unblocks the active phase.

## Week 1 — Runtime Probe Skeleton

- Development: C++20/CMake `runtime_probe` target.
- Learn: CMake targets, C++20 RAII, CLI output, Linux environment discovery.
- Experiment: configure/build/run on a clean checkout.
- Benchmark: configure time, build time, probe startup time.
- Record: missing packages and exact commands.

## Week 2 — CUDA Probe

- Development: CUDA capability probe and CUDA event timing.
- Learn: CUDA device, memory properties, Stream, Event, async copy, synchronization.
- Experiment: one stream timing test and one two-stream concurrency test.
- Benchmark: event overhead and simple CPU-vs-CUDA timing.
- Record: driver/runtime versions and failure modes.

## Week 3 — Vulkan Probe

- Development: Vulkan instance and physical-device probe.
- Learn: instance, validation layers, physical device, queue family, memory heap, driver properties.
- Experiment: enumerate devices and validation-layer availability.
- Benchmark: enumeration latency and validation overhead where possible.
- Record: selected device and constraints for future rendering.

## Week 4 — Metrics and Phase Gate

- Development: structured metrics output and repeatable benchmark script/command.
- Learn: p50/p95/p99, warmup, sample size, clock choice, JSON/log format.
- Experiment: repeated probe runs.
- Benchmark: stable probe report and timing variance.
- Record: Phase 1 review and Phase 2 entry decision.
