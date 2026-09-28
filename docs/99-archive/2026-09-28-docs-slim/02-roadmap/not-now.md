# Not Now

Effective 2026-09-21 (ADR-0007). These topics are valid future directions, but they are not current work.

## Do Not Build Now

- Do not train VLA models, World Models, or world foundation models.
- Do not build a complete physics engine, game engine, CARLA/Isaac/Unreal/Godot replacement, editor, asset pipeline, scripting system, DDS implementation, or multiplayer framework.
- Do not build a full ROS 2 runtime or ROS 2 bridge (backlogged to Phase 3 re-evaluation at the earliest).
- Do not build LLM/VLA/Brain integration beyond the Phase-0 minimal Action contract.
- Do not build the full Vulkan renderer / media pipeline (FFmpeg/NVDEC/NVENC): both return only when a phase's benchmark path requires them — that is not Phase 0.
- Do not build industrial digital-twin or robot-fleet functionality: that is NVIDIA Mega's territory (see direction-decision report §4.2), and entering it is out of scope by design.
- Do not build OpenUSD compliance, import, or round-trip (one-way export remains an Exploration topic only).
- Do not build games/application demos, digital-human systems, mobile/Android/Jetson support, or distributed/multi-node runtime.
- Do not do platform evangelism (docs site, talks, community forum) before Gate 2 produces external-evidence.

A shorter operational rule: if a feature's contribution cannot be named as "a metric only OmniWorld can measure", it does not enter Core.

## Do Not Study Now Unless Required by Current Code

- Advanced robotics control, VLA training papers, neural rendering / Gaussian Splatting, ray tracing beyond basic awareness, OpenCL portability, large-scale simulation platforms, LLM agent frameworks.

## Re-entry Rule

A Not Now item can return only through this sequence: Backlog → Research Queue → master-roadmap phase → current-sprint → Code/Experiment/Benchmark.
