# OmniWorld Engine Development Plan

## Single Main Route

```text
Current Focus: Phase 1 — GPU Runtime Foundation
      ↓
Next Focus: Phase 2 — GPU Video Pipeline
      ↓
Later: Vulkan Renderer → AI Runtime → World State → Brain
      ↓
Backlog: ROS 2 / Simulation / VLA / Digital Human / World Model training
```

Only one project may be `ACTIVE` at a time. New technologies first enter `docs/17-research/research-queue.md`; new product ideas first enter `BACKLOG.md`.

## Phase 0 — Environment and Infrastructure

- Goal: establish repository structure, documentation authority, benchmark discipline, and Linux/NVIDIA-first assumptions.
- Learning: Git workflow, Markdown maintenance, benchmark note-taking, CMake basics.
- Development: documentation skeleton, project folders, benchmark templates.
- Main tech: Git, Markdown, CMake, Linux, NVIDIA driver awareness.
- Output: coherent repo entry points and archived legacy plans.
- Benchmark: none beyond reproducible environment notes.
- Completion standard: README, architecture, roadmap, knowledge, project, and benchmark scaffolds exist and agree on scope.
- Entry condition for next phase: planning no longer blocks creating a runnable C++ target.

## Phase 1 — GPU Runtime Foundation

- Goal: create the first runnable OmniWorld Runtime Probe.
- Learning: C++20, CMake/Ninja, Linux device discovery, CUDA device model, CUDA Stream/Event, Vulkan physical device/queue/memory basics.
- Development: runtime skeleton, logging, metrics, CUDA probe, Vulkan probe, configuration output.
- Main tech: C++20, CMake, CUDA Runtime API, Vulkan loader/SDK, Linux.
- Output: `runtime_probe` CLI reporting CPU, GPU, CUDA, Vulkan, VRAM, driver, and runtime configuration.
- Benchmark: probe startup latency, CUDA event timing overhead, Vulkan enumeration latency.
- Completion standard: clean configure/build/run works; output is recorded; failure modes are explicit.
- Entry condition for next phase: GPU and API capabilities are known and benchmark commands are repeatable.

## Phase 2 — GPU Video Pipeline

- Goal: move decoded frames through GPU processing and toward presentation without unmeasured copies.
- Learning: FFmpeg demux, NVDEC concepts, frame lifecycle, CUDA image kernels, buffer pools, timestamps, backpressure.
- Development: video input path, GPU frame abstraction, decode/probe path, CUDA processing stage, frame metrics.
- Main tech: FFmpeg, NVDEC/Video Codec SDK, CUDA, C++ buffer pools.
- Output: video frame path with CPU-vs-GPU processing benchmark.
- Benchmark: decode throughput, frame latency p50/p95/p99, copy count, drops, queue depth.
- Completion standard: 1080p60 target path is measured; 4K experiment is recorded; copies/sync waits are classified.
- Entry condition for next phase: a stable GPU Frame contract exists.

## Phase 3 — Vulkan Renderer

- Goal: display GPU frames and overlays through the minimal renderer required by OmniWorld.
- Learning: Vulkan memory, queues, command buffers, swapchain, synchronization, image layout transitions.
- Development: Vulkan texture/display path, import/upload decision, render pass or dynamic rendering, simple overlay primitives.
- Main tech: Vulkan, CUDA/Vulkan interop where justified, validation layers.
- Output: GPU Frame → Vulkan Texture → Render path.
- Benchmark: present latency, GPU/CPU sync waits, frame pacing, validation status.
- Completion standard: processed frames render reliably with documented memory/sync behavior.
- Entry condition for next phase: rendering can consume the same GPU Frame contract from Phase 2.

## Phase 4 — AI Runtime

- Goal: run measured inference on the video/render path.
- Learning: TensorRT runtime, CUDA streams, preprocessing, dynamic shapes, engine lifecycle.
- Development: model loader, preprocess stage, async inference stage, result buffer, renderer overlay connection.
- Main tech: TensorRT, CUDA, optional CV-CUDA, benchmark harness.
- Output: Video → GPU → TensorRT → Detection → Vulkan Overlay demo.
- Benchmark: inference latency, FPS, VRAM, throughput, preprocess cost, end-to-end latency.
- Completion standard: inference results are timestamped, rendered, and benchmarked independently and end-to-end.
- Entry condition for next phase: perception output has a stable data contract.

## Phase 5 — World State

- Goal: convert perception results into a minimal runtime world representation.
- Learning: state schemas, timestamps, entity/component tradeoffs, observation/action/transition contracts.
- Development: WorldState, Observation, Action, Transition logs, perception-to-world update path.
- Main tech: C++ data modeling, serialization, metrics.
- Output: perception updates WorldState and renderer visualizes state.
- Benchmark: update latency, serialization cost, memory growth, jitter.
- Completion standard: world updates are deterministic enough for replay and benchmark review.
- Entry condition for next phase: Brain can consume Observation and produce Action without knowing renderer/video internals.

## Phase 6 — Brain / Agent

- Goal: close a minimal Observation → Brain → Action → World loop.
- Learning: provider boundaries, mock/local/remote model APIs, action validation, latency budgets.
- Development: Brain interface, mock provider, local/remote adapter boundary, action executor.
- Main tech: C++ interfaces, HTTP/gRPC only if needed, model-agnostic adapter design.
- Output: Brain loop demo with measurable action latency.
- Benchmark: decision latency, timeout rate, action application latency.
- Completion standard: mock Brain and one provider path operate through the same contract.
- Entry condition for next phase: external ecosystems can be adapters, not core dependencies.

## Phase 7 — ROS 2 / Simulation

- Goal: connect robotics and simulation ecosystems without turning OmniWorld into a ROS 2 or simulator clone.
- Learning: ROS 2 nodes/topics, REP-2007/2009, NITROS basics, simulation boundaries.
- Development: ROS 2 adapter, simulation adapter, timestamp and memory-boundary metrics.
- Main tech: ROS 2, rclcpp, NITROS, CARLA/Isaac only as adapters.
- Output: adapter demo with measured data boundary.
- Benchmark: adapter latency, copy count, dropped messages, clock drift.
- Completion standard: adapter limitations are measured and documented.
- Entry condition for next phase: embodied AI demos can reuse runtime, world, and adapter contracts.

## Phase 8 — VLA / Physical AI

- Goal: explore VLA/WAM/Physical AI applications on top of the completed runtime path.
- Learning: VLA inference, robot policy evaluation, simulation-to-real constraints, safety boundaries.
- Development: VLA adapter or application demo only; no model training platform.
- Main tech: external VLA models, ROS 2/simulation adapters, WorldState/Brain contracts.
- Output: measured physical-AI demo or research report.
- Benchmark: policy latency, action success rate, safety failures, sim/runtime synchronization.
- Completion standard: demo proves value without rewriting the engine core.
- Entry condition for future work: benchmark evidence justifies deeper investment.
