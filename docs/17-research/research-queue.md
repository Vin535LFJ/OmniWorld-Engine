# Research Queue

Research does not change the roadmap by itself. A topic must produce code, experiment, or benchmark evidence before it can become architecture.


| Topic | Priority | Track | Status | Why | When |
| --- | --- | --- | --- | --- | --- |
| CUDA Graph | High | Engineering | Exploration | May reduce runtime scheduling overhead after basic streams/events are measured. | Phase 1/2 |
| CUDA external memory / semaphore interop | High | Engineering | Exploration | Required only if CUDA/Vulkan sharing beats simpler paths. | Phase 2/3 |
| Vulkan Video | Medium | Engineering | Research | May matter for future decode paths, but NVDEC/FFmpeg is more practical first. | Phase 2 research |
| CV-CUDA | Medium | Engineering | Research | Could accelerate preprocessing before TensorRT. | Phase 4 |
| TensorRT dynamic shapes | Medium | Engineering | Research | Needed if AI input sizes vary. | Phase 4 |
| ROS 2 NITROS | Low | Intelligence | Exploration | Relevant as an adapter after WorldState and boundary contracts exist. | Phase 7 |
| World Model API experiment | Low | Intelligence | Exploration | Can test observation/action concepts without entering Core. | Exploration budget |
| Agent / Brain provider prototype | Low | Intelligence | Exploration | Can validate decision latency concepts without changing Runtime. | Exploration budget |
| CARLA / Isaac adapter boundary | Low | Intelligence | Research | Useful for simulation demos, not core runtime. | Phase 7 |
| VLA / WAM | Low | Intelligence | Exploration | Application layer, not foundation. | Phase 8 |
| Gaussian Splatting / Neural Rendering | Low | Intelligence | Backlog | Future rendering research, not current renderer MVP. | Backlog |
