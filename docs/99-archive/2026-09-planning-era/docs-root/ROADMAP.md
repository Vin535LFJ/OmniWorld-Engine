# Roadmap

## MVP ladder

| MVP | Goal | Scope | Non-goals | Exit criteria |
| --- | --- | --- | --- | --- |
| MVP-0 | Minimal GPU runtime skeleton | C++20, CMake, CUDA probe, Vulkan probe, metrics API, resource handles | Video, AI, ROS, editor | Builds locally; records version info; one CUDA and one Vulkan smoke test |
| MVP-1 | Minimal GPU dataflow | Video demux/decode path, CUDA processing, Vulkan display, timestamps, buffer pool | TensorRT, ROS, world model | Benchmarks report throughput, latency, copies, waits |
| MVP-2 | AI inference dataflow | GPU preprocess, TensorRT engine execution, overlay results | Training, multi-model platform | Inference stage measured independently and end-to-end |
| MVP-3 | World Runtime | WorldState, Observation, Action, perception-to-render path | Full simulator/physics | Perception result updates WorldState rendered as observation |
| MVP-4 | Brain Runtime | Brain interface, local/mock/remote provider adapters | VLA training | Observation→Brain→Action→World loop demo |
| MVP-5 | Ecosystem adapters | ROS 2/NITROS, simulation, VLA integration | ROS replacement, CARLA clone | Adapter demos prove boundary and latency semantics |

## 12-month roadmap

### Quarter 1: Foundation and truth-first GPU dataflow
- Core capability: C++20 build skeleton, version policy, logging/metrics/profiling, CUDA/Vulkan capability probes.
- Demo: `gpu_probe` and a synthetic CUDA→Vulkan texture experiment.
- Benchmark: smoke latency, allocation count, CPU waits, validation-layer status.

### Quarter 2: Video → CUDA → Vulkan MVP
- Core capability: demux/decode integration, GPU frame abstraction, buffer pool, CUDA stream processing, Vulkan display bridge.
- Demo: video frame processed by CUDA and displayed by Vulkan.
- Benchmark: p50/p95/p99 stage latency, copy classification, frame drops, CPU utilization.

### Quarter 3: AI inference runtime
- Core capability: preprocessing boundary, TensorRT model abstraction, asynchronous inference, result buffers, overlay path.
- Demo: object detection or segmentation on video with GPU-resident processing where possible.
- Benchmark: inference/s, preprocess→inference latency, GPU memory, synchronization waits.

### Quarter 4: World State and public narrative
- Core capability: minimal WorldState, Observation, Action, renderer observation path, published benchmark reports.
- Demo: perception updates WorldState and renderer visualizes state.
- Benchmark: end-to-end video→perception→world→render latency and jitter.

## Three-year roadmap

### Year 1 — Runtime MVP
Build a verified GPU-native runtime MVP centered on media, compute, inference, Vulkan display, profiling, and benchmark discipline.

### Year 2 — World Runtime and adapters
Add robust WorldState, Brain interface, ROS 2/NITROS adapter experiments, lightweight simulation integration, and reproducible application demos.

### Year 3 — Embodied/Physical AI applications
Explore VLA/robotics/simulation applications only if Year 1–2 runtime measurements justify it. Keep VLA, CARLA, Isaac, PhysX, and LLMs as adapters or applications.
