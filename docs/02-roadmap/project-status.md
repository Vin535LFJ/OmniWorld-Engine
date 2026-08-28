# Project Status

Only one project may be `ACTIVE` at a time.

| Project | Status | Why | Promotion condition |
| --- | --- | --- | --- |
| GPU Runtime Foundation | ACTIVE | The engine has no runnable core yet; every later phase depends on capability, logging, metrics, memory, and synchronization basics. | Complete Runtime Probe Definition of Done. |
| gpu-video-engine | NEXT | It is the first real dataflow using the runtime foundation. | Runtime Probe reports CUDA/Vulkan capabilities and metrics reliably. |
| vulkan-renderer | NEXT | Rendering is needed after GPU Frame exists; do not build it before the frame contract. | GPU Frame abstraction and processing path are measured. |
| ai-runtime | NEXT | AI inference depends on GPU frame and metrics contracts. | Video pipeline and renderer bridge provide stable input/output. |
| world-engine | LATER | WorldState needs perception/render outputs first. | AI results have stable schemas and timestamps. |
| ros2-adapter | LATER | ROS 2 must remain an adapter after engine contracts exist. | WorldState/Observation/Action contracts are stable. |
| simulation-adapter | RESEARCH | CARLA/Isaac choices require evidence and should not drive core architecture now. | Research queue produces adapter boundary evidence. |
| brain-runtime | LATER | Brain needs WorldState and Action contracts. | Minimal WorldState loop exists. |
| vla / physical-ai apps | BACKLOG | Too far from current runtime foundation and risks derailing execution. | Phase 7 adapter benchmarks justify Phase 8. |
| digital-human | BACKLOG | Interesting application but not core runtime work. | Engine has mature video/render/AI/world loop. |
