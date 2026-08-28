# System Architecture

## What OmniWorld Engine is

OmniWorld Engine is a runtime/infrastructure project for real-time GPU dataflow and world-state integration. Its value is in explicit ownership of memory, scheduling, synchronization, profiling, and state transitions across media, rendering, perception, inference, and agent loops.

## Canonical system diagram

```text
┌───────────────────────────────────────────────┐
│                 Applications                  │
│ AD / Robotics / Digital Human / AI / XR demos │
└───────────────────────┬───────────────────────┘
                        │
┌───────────────────────▼───────────────────────┐
│          Brain / Adapter Layer                 │
│ LLM / VLM / VLA / WAM / Agent / API Adapter   │
└───────────────────────┬───────────────────────┘
                        │
┌───────────────────────▼───────────────────────┐
│                  World Layer                   │
│ WorldState / Observation / Action / Transition │
└───────────────────────┬───────────────────────┘
                        │
┌───────────────────────▼───────────────────────┐
│             AI / Perception Layer              │
│ TensorRT / CUDA / CV-CUDA / Vision             │
└───────────────────────┬───────────────────────┘
                        │
┌───────────────────────▼───────────────────────┐
│                Rendering Layer                 │
│ Vulkan / Compute / Render Graph / PostFX       │
└───────────────────────┬───────────────────────┘
                        │
┌───────────────────────▼───────────────────────┐
│                  GPU Runtime                   │
│ Memory / Scheduling / Synchronization / Metrics│
└───────────────────────┬───────────────────────┘
                        │
┌───────────────────────▼───────────────────────┐
│               Media / Data Layer               │
│ NVDEC / NVENC / FFmpeg / Camera / Sensor       │
└───────────────────────┬───────────────────────┘
                        │
┌───────────────────────▼───────────────────────┐
│                Platform Layer                  │
│ Linux first / NVIDIA GPU first / Jetson later  │
└───────────────────────────────────────────────┘
```

## Term boundaries

| Term | Definition | Boundary |
| --- | --- | --- |
| Engine | The full repo-level system: runtime, docs, knowledge, experiments, demos | Not a commercial game engine |
| Runtime | The reusable execution substrate for memory, scheduling, synchronization, metrics, and lifecycle | Core project value |
| Renderer | A Vulkan-backed observation and visualization module | Not an editor or full asset pipeline |
| GPU Runtime | Memory/resource handles, device queues, CUDA/Vulkan sync, profiling hooks | Must not depend on ROS 2 or VLA |
| Video Pipeline | Camera/file/network ingest, demux, decode, GPU frame lifecycle, encode | Uses codec libraries; does not implement codecs |
| AI Runtime | Preprocess, TensorRT execution, model lifecycle, inference metrics | Does not train foundation models |
| World State | Structured state representation updated by perception/simulation/actions | Not a learned world model by default |
| Simulation | External or lightweight provider of synthetic observations/transitions | Adapter/application, not core |
| Brain / Model Adapter | LLM/VLM/VLA/local/remote policy provider | No GPU resource ownership in core |
| ROS 2 Adapter | Boundary translating Engine data to ROS 2 graphs | Never required by Engine Core |
| VLA Adapter | Model-specific action provider | No training stack in core |
| Environment | Real or synthetic source/sink of observations and actions | Replaceable provider |
| Agent | Entity choosing actions from observations/goals | Consumes interfaces, does not own runtime |
