# OmniWorld Engine Architecture

## Canonical architecture

```text
External inputs/providers
  ├─ Real world: video, camera, sensors
  └─ Synthetic world: renderer/simulator/physics adapter
        │
        ▼
Engine Core: GPU Data Runtime
  ├─ foundation: lifecycle, errors, logging, configuration, metrics
  ├─ memory: GPU buffers/images, pools, ownership, handles
  ├─ scheduling: execution graph, queues, backpressure, timestamps
  ├─ synchronization: CUDA events, Vulkan semaphores, fences, barriers
  ├─ media: demux/decode/encode integration boundaries
  ├─ compute: CUDA kernels, CV-CUDA integration boundary
  ├─ inference: TensorRT runtime boundary
  └─ world: WorldState, Observation, Action, WorldTransition
        │
        ├──────────────► Renderer: Vulkan observation/display path
        │
        └──────────────► Brain interface: LLM/VLM/VLA/local/remote providers
                            │
                            ▼
                         Action
                            │
                            ▼
                     World transition/update
```

## Core rule

Engine Core owns runtime abstractions and measured dataflow semantics. Adapters translate external ecosystems into or out of those abstractions. External systems never become foundational dependencies of Engine Core.

## Canonical project structure

```text
OmniWorld-Engine/
├── cmake/
├── docs/
│   ├── architecture/
│   ├── benchmarks/
│   ├── decisions/
│   ├── plans/
│   ├── research/
│   └── backlog/
├── engine/
│   ├── foundation/
│   ├── gpu/
│   ├── memory/
│   ├── scheduler/
│   ├── media/
│   ├── inference/
│   ├── renderer/
│   ├── world/
│   └── brain/
├── adapters/
│   ├── ros2/
│   ├── nitros/
│   ├── llm/
│   ├── carla/
│   └── isaac/
├── apps/
│   ├── gpu_video_demo/
│   ├── vision_demo/
│   ├── world_demo/
│   └── embodied_demo/
├── tests/
├── benchmarks/
├── samples/
├── scripts/
└── tools/
```

This structure is a planning target. Directories should be created only when implementation begins.

## Dependency graph

```text
Foundation [hard]
  ├─ GPU Memory [hard for media/AI/renderer]
  │   ├─ Media Runtime [hard for MVP-1]
  │   ├─ Compute Runtime [hard for MVP-1]
  │   ├─ AI Runtime [hard for MVP-2]
  │   └─ Vulkan Renderer [hard for display/observation]
  ├─ Scheduler/Dataflow [hard for all MVPs]
  └─ Metrics/Profiling [hard for acceptance]

World Runtime [soft until MVP-3]
  ├─ Brain Runtime [soft until MVP-4]
  ├─ Simulation Adapter [replaceable/external]
  └─ ROS 2 Adapter [external, replaceable]
```

## Build vs modularity decision

The canonical form is a single monorepo with separately buildable modules. This is better than many repositories for one developer because shared CI, benchmarks, dependency versions, docs, and examples remain consistent. It is better than a single monolithic phase because each module can be mocked, benchmarked, and replaced.
