# Backlog / Future Research

Effective 2026-09-21 (ADR-0007). Backlog is not the roadmap. Items here are explicitly out of current implementation scope until [`docs/02-roadmap/master-roadmap.md`](docs/02-roadmap/master-roadmap.md) promotes them into a phase and `CURRENT.md` names them as current work.

## Rules

1. If an idea enters Backlog, do not implement it in the current phase unless it is required to finish the active Definition of Done.
2. Exploration exception: a Backlog item may move to bounded Exploration only under `docs/03-engineering-lifecycle/exploration-rules.md`. Exploration cannot modify Core or replace the ACTIVE project.
3. Since 2026-09-21 the planning phase is closed: new ideas go to this file first; nothing expands the current sprint in place.

## Classification of technologies (for orientation)

| Technology | Lane | Reason |
| --- | --- | --- |
| CUDA | CORE | compute foundation of the loop |
| World Contract + loop metrics | CORE | the thesis itself |
| ONNX Runtime | ADAPTER | Phase-0 perception path (TensorRT deferred until a benchmark needs it) |
| TensorRT | ADAPTER (later) | fast inference path once Phase-1+ models justify it |
| FFmpeg / NVDEC / NVENC | ADAPTER (deferred) | media input when a phase's benchmark requires it |
| Replay data / tiny dynamics model | ADAPTER | world-model participant, Phase 1 |
| External physics (Jolt/Rapier class) | ADAPTER (Phase 3) | second truth world |
| Vulkan | BACKLOG | display/renderer path is not in the loop thesis |
| OpenUSD | EXPLORATION | one-way export only |
| World models / generative world models | ADAPTER / evaluation subject | they are the studied participants, never Core |
| ROS 2 | BACKLOG | adapter only, ≥6 months |
| VLA / LLM / Brain | BACKLOG | above the minimal Action contract |
| Neural rendering / Gaussian Splatting | EXPLORATION | future rendering research |
| Agent | CORE (minimal contract) + BACKLOG (rich integration) | Action validation lives in Core; brains live outside |

## Backlog Topics

- ROS 2 bridge (NITROS or plain), CARLA / Isaac Sim adapters, VLA / Physical AI / WAM applications.
- World-model or world-foundation-model training; heavy generative pipelines.
- Digital Human runtime; Gaussian Splatting; neural rendering; ray tracing beyond basic awareness.
- Android/mobile runtime; Jetson deployment; distributed runtime and cluster scheduling; multi-user/networked simulation.
- Full physics engine; editor, scripting, asset pipeline, game-engine feature set; Unreal/O3DE/Godot plugins.
- World Labs World API adapter (optional scene-content adapter; NEVER in the reproducible benchmark mainline).
- OpenUSD one-way export (also listed as an Exploration topic in the research queue).
