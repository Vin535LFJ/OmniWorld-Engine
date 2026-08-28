# Execution Plan

## First 12 weeks

| Week | Learning | Coding | Experiment | Benchmark | Documentation | Exit criteria |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | CMake/C++20/CUDA/Vulkan setup docs | Build skeleton only | Compile CUDA and Vulkan probes | Record tool versions | Update TECH_STACK | Reproducible configure/build plan |
| 2 | CUDA resource lifetime and streams | Logging, errors, timers | CUDA event timing | Timer overhead | Runtime notes | Timers validated |
| 3 | Vulkan instance/device/swapchain basics | Vulkan smoke app | Validation layers | Frame timing | Vulkan notes | Triangle or clear-screen app |
| 4 | CUDA/Vulkan external memory docs | Interop prototype skeleton | Buffer export/import | Wait/signal timing | Zero-copy notes | Gate 1 decision data |
| 5 | Video Codec SDK/NVDEC | Demux/decode spike | Decode sample video | Decode throughput | Media notes | GPU-accessible frame path observed |
| 6 | Frame lifecycle/backpressure | Frame abstraction design | Ring buffer | Drops/queue depth | Dataflow doc | Buffer lifecycle accepted |
| 7 | CUDA image kernels/CV-CUDA | Simple GPU transform | Process decoded/synthetic frames | Kernel latency | Compute notes | Transform measured |
| 8 | Vulkan texture upload/import choices | Display processed frame | Present path variants | Render latency | Decision note | MVP-1 path selected |
| 9 | TensorRT runtime basics | Model loader spike | Static image inference | enqueue latency | AI notes | MVP-2 feasibility decided |
| 10 | TensorRT streams/dynamic shapes | Async inference path | Video-frame inference mock | Stage latency | Inference doc | No unmeasured sync in happy path |
| 11 | WorldState modeling | Minimal WorldState schema | Perception result→state | Update latency | world-model update | MVP-3 API approved |
| 12 | Benchmark methodology | Harness skeleton | End-to-end dry run | p50/p95/p99 report | Benchmark report template | First public milestone scoped |

## Codex operating rule

Each task must include files likely affected, benchmark or validation command, and Definition of Done. Do not implement broader systems than the task requires.
