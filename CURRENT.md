# Current Focus

Phase: Phase 1 — GPU Runtime Foundation

Current Project: GPU Runtime Foundation

Current Goal: Build the first runnable OmniWorld Runtime Probe that reports CPU, GPU, CUDA, Vulkan, VRAM, driver, and runtime configuration.

## This Week

1. Create the C++20/CMake/Ninja project skeleton.
2. Implement CUDA capability probing.
3. Implement Vulkan physical-device probing.
4. Add basic logging, error handling, and metrics timestamps.
5. Record the first probe output under the benchmark workflow.

## Learn

- C++20 project structure and CMake targets.
- Linux GPU driver/runtime discovery.
- CUDA device model, memory basics, streams, events, and timing.
- Vulkan instance, physical device, queues, memory heaps, and validation layers.

## Do Not Work On

- ROS 2, CARLA, Isaac, or robotics adapters.
- VLA, World Model training, LLM agent orchestration, or Digital Human.
- Full Vulkan renderer, full video engine, full game engine, or distributed runtime.
- Any new technology that is not required by the Runtime Probe.

## Definition of Done

Runtime Probe builds and runs locally, prints CPU/GPU/CUDA/Vulkan/VRAM/driver/configuration data, and has at least one repeatable benchmark/check command recorded.
