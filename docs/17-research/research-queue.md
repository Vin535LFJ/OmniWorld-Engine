# Research Queue

Research does not change the roadmap by itself. A topic must produce code, experiment, or benchmark evidence before it can become architecture.

| Topic | Priority | Why | When |
| --- | --- | --- | --- |
| CUDA Graph | High | May reduce runtime scheduling overhead after basic streams/events are measured. | Phase 1/2 |
| CUDA external memory / semaphore interop | High | Required only if CUDA/Vulkan sharing beats simpler paths. | Phase 2/3 |
| Vulkan Video | Medium | May matter for future decode paths, but NVDEC/FFmpeg is more practical first. | Phase 2 research |
| CV-CUDA | Medium | Could accelerate preprocessing before TensorRT. | Phase 4 |
| TensorRT dynamic shapes | Medium | Needed if AI input sizes vary. | Phase 4 |
| ROS 2 NITROS | Low | Relevant only after WorldState and adapter boundaries exist. | Phase 7 |
| CARLA / Isaac adapter boundary | Low | Useful for simulation demos, not core runtime. | Phase 7 |
| VLA / WAM | Low | Application layer, not foundation. | Phase 8 |
| Gaussian Splatting / Neural Rendering | Low | Future rendering research, not current renderer MVP. | Backlog |
