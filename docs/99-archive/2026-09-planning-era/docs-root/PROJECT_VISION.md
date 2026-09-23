# OmniWorld Engine Project Vision

## One-sentence definition

OmniWorld Engine is a GPU-native C++20 runtime for moving video, compute, AI inference, rendering observations, and world-state updates through GPU systems with the least practical copies, synchronization, and latency.

## Strategic boundary

OmniWorld Engine is **not** a game engine, an autonomous-driving stack, a ROS 2 replacement, a simulator replacement, a physics engine, or a model-training platform. Its core product is the runtime layer that makes real-time world dataflow measurable and controllable.

## Technical thesis

The project exists to answer one engineering question:

> How do we move data through GPU systems with the least possible copies, synchronization, and latency?

Everything else is subordinate to that question.

## Source of Truth Matrix

| Topic | Current statement | Source | Confidence | Status |
| --- | --- | --- | --- | --- |
| Positioning | AI Real-Time World Engine, not Unreal/Apollo replacement | `README.md`, `docs/discussion_1.md` | High | Keep, make stricter |
| Core advantage | GPU dataflow/runtime, not renderer spectacle | `README.md`, `docs/discussion_1.md`, `docs/discussion_3.md` | High | Keep |
| Zero-copy claims | 0 CPU copy / 0 sync / <6 ms / 150+ FPS | `README.md`, `docs/discussion_3.md` | Low | Revise to targets until measured |
| Vulkan strategy | Vulkan external memory/semaphores for CUDA interop | `README.md`, NVIDIA CUDA docs, Vulkan Guide | Medium | Keep as experiment, gate it |
| CUDA strategy | NVIDIA-first runtime using CUDA streams/graphs | `README.md`, TensorRT/CUDA docs | High | Keep |
| NVDEC strategy | Hardware decode feeding GPU-accessible frames | `README.md`, NVIDIA Video Codec SDK docs | Medium | Keep as preferred path, verify memory ownership |
| TensorRT strategy | Async inference on CUDA streams | `README.md`, TensorRT docs | High | Keep, note sync exceptions |
| ROS 2 role | Adapter only, possibly NITROS/type adaptation | `README.md`, REP-2007/2009, NITROS docs | High | Keep as external adapter |
| VLA/LLM role | Brain provider consuming observations and producing actions | `docs/discussion_2.md` | High | Keep as adapter/application |
| World State | Differentiator above plain GPU pipeline | `docs/discussion_2.md` | Medium | Keep, define minimal model |
| Project shape | Five phases vs main project plus subprojects | `README.md`, `docs/deep-research-report.md` | Medium | Revise to monorepo with gated modules |
| Performance numbers | Example comparisons such as 45 ms vs 4 ms | `README.md`, `docs/discussion_3.md` | Low | Reclassify as illustrative/unsupported |

## Classification

### A. Confirmed facts

- The repository currently contains planning/research documentation rather than engine implementation.
- External official documentation supports CUDA external memory/semaphore APIs, Vulkan external memory extensions, TensorRT CUDA-stream enqueue, ROS 2 type adaptation/negotiation, and NITROS-style GPU-accelerated ROS graphs.

### B. Architecture decisions

- Engine Core remains independent from ROS 2, CARLA, Isaac Sim, LLM APIs, VLA models, and physics engines.
- The primary runtime story is GPU Foundation → GPU Dataflow → Media → AI Inference → World State → Renderer/Observation → Brain/Action.
- Monorepo is preferred for coordinated evolution and benchmark reproducibility.

### C. Assumptions to verify

- NVDEC frame layouts and ownership can be integrated cleanly with CUDA processing and Vulkan presentation.
- CUDA/Vulkan external memory is stable enough for the MVP on target Linux/NVIDIA systems.
- NITROS semantics are sufficient for the adapter-level data-sharing use cases.

### D. Performance targets, not results

Any latency, FPS, CPU, PCIe, or synchronization number in historical documents is a target or illustration until produced by `benchmarks/` and recorded with hardware/software versions.

### E. Risks

- Scope creep into full game engine, autonomous-driving stack, simulator, or AI-training platform.
- Over-promising zero-copy semantics across API/process/network boundaries.
- Spending too early on ROS 2/VLA before the GPU dataflow core is proven.

### F. Candidate deletions/deferments

- Editor, scripting language, asset pipeline, multiplayer, DDS implementation, model training stack, full physics engine, full simulator, full autonomous-driving stack.
