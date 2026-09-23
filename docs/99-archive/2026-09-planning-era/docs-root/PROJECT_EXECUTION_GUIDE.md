# Project Execution Guide

## 1. Current Stage

OmniWorld Engine is in **Phase 1 — GPU Runtime Foundation**. Phase 0 planning and scaffolding are sufficient; the project now needs a runnable runtime probe, not more broad planning.

## 2. Learn Now

- C++20 project structure, RAII, error/result handling, and small CLI design.
- CMake/Ninja configure/build/test workflow.
- CUDA device model, memory properties, Stream, Event, async copy, and synchronization.
- Vulkan instance, physical device, queue families, memory heaps, driver properties, and validation layers.
- Benchmark basics: warmup, repeated samples, p50/p95/p99, timing overhead, and reproducible logs.

## 3. Build Now

- `runtime_probe` executable.
- CPU/OS/runtime configuration probe.
- CUDA capability and timing probe.
- Vulkan physical-device and validation-layer probe.
- Logging, metrics, and one repeatable benchmark/check command.

## 4. Do Not Touch Now

- ROS 2, CARLA, Isaac, robotics adapters.
- VLA, WAM, World Model training, LLM/VLM agent orchestration.
- Digital Human, neural rendering, Gaussian Splatting, advanced ray tracing.
- Full game engine, editor, physics engine, asset pipeline, mobile, Jetson, or distributed runtime.

## 5. Next 7 Days

1. Create the C++20/CMake skeleton.
2. Add a `runtime_probe` target.
3. Print CPU/OS/runtime configuration.
4. Add CUDA device probe.
5. Add Vulkan physical-device probe.
6. Add basic logging/metrics.
7. Record one clean build/run result.

## 6. Next 30 Days

Finish the Runtime Probe. It must report CPU, GPU, CUDA, Vulkan, VRAM, driver, and runtime configuration; run from a clean checkout; and include repeatable timing/benchmark output.

## 7. Next 90 Days

- Days 0–30: Runtime Probe.
- Days 31–60: GPU Video Pipeline: Video → Decode → GPU Frame → CUDA.
- Days 61–90: Integration spike: Video → GPU → TensorRT → Detection → Vulkan Overlay.

## 8. Six-Month Target

A measured GPU video pipeline exists with a stable GPU Frame contract, CUDA processing, a minimal Vulkan display path, and benchmark reports covering latency, copies, synchronization waits, drops, and VRAM.

## 9. Twelve-Month Target

A Year-1 Runtime MVP exists: media input, GPU processing, Vulkan rendering, TensorRT inference, minimal WorldState update, end-to-end metrics, and public benchmark reports.

## 10. Two-to-Three-Year Target

OmniWorld Engine becomes a reusable GPU-native runtime with Runtime, GPU, Video, Renderer, AI, World, Brain, ROS2, and Simulation layers. VLA/Physical AI remains an application/adaptation layer built on proven engine contracts rather than a distraction from core runtime development.


## Lifecycle Rule

Use `docs/03-engineering-lifecycle/lifecycle.md` for L/XL work, `docs/03-engineering-lifecycle/project-sizing.md` to scale process depth, and `docs/03-engineering-lifecycle/exploration-rules.md` before any ROS 2 / World Model / Agent / VLA exploration.


## Developer Briefing

Current Phase: Phase 1 — GPU Runtime Foundation
Current Project: GPU Runtime Foundation
Current Goal: Build OmniWorld Runtime Probe.

Learn Now:
1. C++20/CMake/Linux runtime skeleton.
2. CUDA device, memory, Stream, Event, synchronization.
3. Vulkan physical device, queue, memory, validation layers.

Build Now:
1. `runtime_probe` CLI.
2. CUDA and Vulkan capability probes.
3. Logging, metrics, and repeatable benchmark output.

Do Not Touch:
1. ROS 2 / CARLA / Isaac.
2. VLA / World Model / Digital Human.
3. Full renderer/game engine/distributed runtime.

This Week:
1. Create build skeleton.
2. Implement capability probes.
3. Record first benchmarkable probe output.

Next Milestone: clean build and run of `runtime_probe`.

Definition of Done: Runtime Probe outputs CPU/GPU/CUDA/Vulkan/VRAM/driver/runtime configuration and basic timing metrics with a documented repeatable command.
