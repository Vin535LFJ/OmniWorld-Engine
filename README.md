# OmniWorld Engine

**OmniWorld Engine is a GPU-native real-time runtime that connects perception, media, rendering, simulation, and foundation-model-driven agents through a unified world representation.**

中文：**OmniWorld Engine 是一个 GPU 原生实时 Runtime，通过统一的 World Representation，将视频/传感器、AI 感知、GPU 计算、实时渲染、仿真以及 LLM/VLM/VLA/Agent 连接成一个可闭环运行的系统。**

## Vision

Build a long-term engineering platform for learning and demonstrating how real-world and synthetic-world data can move through GPU systems with minimal practical copies, explicit synchronization, measurable latency, and clean runtime boundaries.

## Why

Modern AI systems increasingly combine video, vision, rendering, simulation, and agents. The hard infrastructure problem is not merely "using a GPU"; it is controlling memory ownership, synchronization, scheduling, profiling, and data contracts across CUDA, Vulkan, video codecs, TensorRT, and external robotics/model ecosystems.

## What It Is

OmniWorld Engine is planned as:

- a C++20/Linux-first GPU runtime;
- a benchmark-driven media, compute, inference, rendering, and world-state dataflow system;
- a learning-oriented knowledge base tied to experiments, benchmarks, and project modules;
- an adapter-friendly infrastructure layer for ROS 2, simulation, LLM/VLM/VLA providers, and robotics demos.

## What OmniWorld Engine Is NOT

- Not an Unreal Engine replacement.
- Not a complete Apollo/Autoware autonomous-driving stack.
- Not a CARLA or Isaac Sim replacement.
- Not a VLA, LLM, or world-model training project.
- Not a pure video player.
- Not a pure renderer.
- Not a ROS 2 project with GPU code bolted on.
- Not a full physics engine, editor, asset pipeline, scripting system, DDS implementation, or multiplayer framework.

## Architecture

```text
Applications / Demos
    │
Brain & Adapter Layer: LLM / VLM / VLA / ROS 2 / Simulation
    │
World Layer: WorldState / Observation / Action / Transition
    │
AI & Perception: CUDA / CV-CUDA / TensorRT / Vision
    │
Rendering: Vulkan / Render Graph / Observation Output
    │
GPU Runtime: Memory / Scheduling / Synchronization / Metrics
    │
Media & Data: Camera / Sensor / FFmpeg / NVDEC / NVENC
    │
Platform: Linux first, NVIDIA GPU first, Jetson later
```

Read the full architecture in [`docs/01-architecture/system-architecture.md`](docs/01-architecture/system-architecture.md).

## Current Status

Current state: **planning, architecture, research organization, and knowledge-base scaffolding**. Engine implementation has not started. Any historical performance or zero-copy numbers are treated as targets or hypotheses until benchmarked in this repository.

## Roadmap

The project progresses through:

1. Knowledge / Foundation
2. GPU Runtime Skeleton
3. GPU Video Pipeline
4. Vulkan Observation Renderer
5. AI Runtime
6. World State
7. Brain / Agent Loop
8. ROS 2 / Simulation / VLA adapters

See [`docs/02-roadmap/master-roadmap.md`](docs/02-roadmap/master-roadmap.md).

## Projects

Subprojects are documented under [`projects/`](projects/). Each project owns its architecture, roadmap, benchmark plan, notes, and local knowledge base.

## Knowledge Base

The long-term learning system lives under [`knowledge/`](knowledge/). It separates project knowledge, external research, hypotheses, experiments, benchmarks, lessons learned, and review prompts.

## Benchmarks

Benchmark plans live under [`benchmarks/`](benchmarks/) and [`docs/16-benchmarks/`](docs/16-benchmarks/). The goal is to answer: **why is this system fast, and where is it slow?**

## Research

External project and technology research lives under [`docs/17-research/`](docs/17-research/). Legacy planning material is preserved under [`docs/99-archive/`](docs/99-archive/).

## How to Build

No engine code exists yet. The first implementation task is to create the C++20/CMake/Ninja/CUDA/Vulkan capability-probe skeleton.

## How to Run

No runnable demo exists yet. The first runnable target should be an MVP-0 environment probe, followed by a CUDA/Vulkan interop experiment.

## Contributing

Keep facts, plans, hypotheses, experiments, and verified results separate. Every new technical claim should be traceable to project evidence or external sources.

## License

License status should be finalized before adding source code or accepting external contributions.
