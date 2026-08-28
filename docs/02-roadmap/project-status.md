# Project Status


Only one project may be `ACTIVE` as the main implementation route. Exploration is allowed only under the rules in `docs/03-engineering-lifecycle/exploration-rules.md`.

| Project / Topic | Status | Track | Why | Promotion condition |
| --- | --- | --- | --- | --- |
| GPU Runtime Foundation | ACTIVE | Engineering | The engine has no runnable core yet; every later phase depends on capability, logging, metrics, memory, and synchronization basics. | Complete Runtime Probe Definition of Done. |
| CUDA | ACTIVE | Engineering | Required for the active Runtime Probe and later GPU processing. | Continue through benchmarks and contracts. |
| Vulkan | ACTIVE/NEXT | Engineering | Required for device probing now and renderer work next. | Renderer work waits for GPU Frame contract. |
| gpu-video-engine | NEXT | Engineering | It is the first real dataflow using the runtime foundation. | Runtime Probe reports CUDA/Vulkan capabilities and metrics reliably. |
| vulkan-renderer | NEXT | Engineering | Rendering is needed after GPU Frame exists; do not build it before the frame contract. | GPU Frame abstraction and processing path are measured. |
| ai-runtime | NEXT | Engineering | AI inference depends on GPU frame and metrics contracts. | Video pipeline and renderer bridge provide stable input/output. |
| World State / world-engine | EXPLORATION | Intelligence | World representation should be studied now, but implementation waits for perception/render contracts. | Small experiments show useful Observation/Action contracts. |
| Agent / Brain Runtime | EXPLORATION | Intelligence | Agent ideas may be explored through tiny prototypes without changing Core. | WorldState contract exists and Architecture Review accepts a Brain boundary. |
| ROS 2 / ros2-adapter | EXPLORATION | Intelligence | ROS 2 can be studied as an adapter, not a core runtime dependency. | Adapter Spike has benchmarked latency/copy boundaries. |
| World Model | EXPLORATION | Intelligence | Useful long-term direction, but only API-level or toy experiments now. | Benchmark and review prove a near-term runtime requirement. |
| VLA / WAM / Physical AI | EXPLORATION | Intelligence | Allowed as bounded reading or Tiny Prototype; no core implementation. | Phase 7 adapter evidence justifies Phase 8 work. |
| simulation-adapter | LATER | Intelligence | CARLA/Isaac choices require evidence and stable WorldState contracts. | Research queue produces adapter boundary evidence. |
| digital-human | BACKLOG | Intelligence | Interesting application but not core runtime work. | Engine has mature video/render/AI/world loop. |

