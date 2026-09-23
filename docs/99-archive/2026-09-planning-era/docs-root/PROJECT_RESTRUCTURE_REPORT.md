# Project Restructure Report

## 1. Original state

The repository was primarily a planning and research repository. It had no source code, build system, tests, runnable examples, benchmark harnesses, or project-local knowledge bases. The strongest asset was the accumulated strategic thinking around GPU-native real-time AI/world runtime engineering.

## 2. Problems found

- README behaved like a long technical report rather than an open-source project homepage.
- Research, planning, roadmap, and architecture claims were mixed together.
- Multiple files repeated the same positioning and roadmap ideas.
- Zero-copy and performance claims were too strong for an unimplemented repository.
- There was no structured learning/review system.
- There was no separation between docs, knowledge, project modules, benchmarks, and archive.

## 3. Archived documents

Historical research, informal discussions, previous generated docs, previous ADRs, and one-off HTML notes were moved into `docs/99-archive/2026-08-legacy-planning/`. They are preserved for traceability and should be distilled rather than deleted.

## 4. Merged knowledge

The repeated project identity was consolidated into a runtime-first thesis: OmniWorld Engine should focus on GPU dataflow, memory ownership, explicit synchronization, profiling, and world-state contracts. Renderer, ROS 2, simulation, VLA, LLM, and world-model capabilities are valuable only when kept behind clean module/adaptor boundaries.

## 5. New architecture

The new architecture is layered: Platform → Media/Data → GPU Runtime → Rendering → AI/Perception → World → Brain/Adapters → Applications. The most important architectural constraint is that Engine Core owns runtime semantics, while external ecosystems enter through adapters.

## 6. New directory structure

The repository now separates:

- `docs/00-overview/` for identity and current status.
- `docs/01-architecture/` for system architecture, scope, principles, and ADRs.
- `docs/02-roadmap/` for phases, milestones, and project matrix.
- `docs/16-benchmarks/` and `benchmarks/` for benchmark planning and future benchmark artifacts.
- `docs/17-research/` for third-party research maps.
- `docs/99-archive/` for preserved legacy knowledge.
- `knowledge/` for long-term learning and review.
- `projects/` for project-local architecture, roadmap, benchmark, notes, and knowledge.
- `templates/` for repeatable note formats.

## 7. Why this design

The design supports the full loop: Research → Knowledge → Design → Implementation → Benchmark → Lessons Learned → Review. It lets the repository function as codebase, technical research archive, learning system, experiment log, planning system, and portfolio.

## 8. Subproject responsibilities

- GPU Video Engine: media ingest/decode/process/display/encode pipeline.
- Vulkan Renderer: minimal observation/display renderer and later world visualization.
- AI Runtime: TensorRT/CUDA/CV-CUDA inference dataflow.
- ROS 2 Adapter: robotics ecosystem bridge outside core.
- World Engine: WorldState, Observation, Action, and integrated demos.

## 9. Knowledge maintenance

Each knowledge area has concepts, architecture, memory, scheduling, synchronization, profiling, optimization, examples, and review folders. New notes should use templates and must label sources as Project Knowledge, External Research, Inference, Hypothesis, Experimental, or Verified.

## 10. Future expansion

Add source code only after Phase 0 exits. The next expansion should be a C++20/CMake/Ninja/CUDA/Vulkan capability-probe skeleton, followed by measured CUDA/Vulkan interop and video dataflow experiments.

## 11. Biggest current risk

The biggest risk is scope expansion: trying to become a game engine, simulator, robotics stack, VLA system, and AI runtime simultaneously before proving the GPU dataflow core.

## 12. Next priority

If only one thing can be done next, implement MVP-0: a minimal build and environment probe that records compiler, OS, NVIDIA driver, CUDA, Vulkan, GPU, and timing/profiling basics.
