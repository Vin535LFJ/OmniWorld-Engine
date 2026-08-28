# Master Roadmap

## Phase 0 — Knowledge / Foundation
Goal: create the repo skeleton, learning system, toolchain policy, and benchmark discipline. Exit: README, docs, knowledge, templates, benchmarks, and project folders exist and are linked.

## Phase 1 — GPU Runtime Skeleton
Goal: prove C++20, CMake, CUDA, Vulkan, logging, metrics, and resource-handle basics. Exit: local build and smoke probes record versions and timing.

## Phase 2 — GPU Video Pipeline
Goal: validate video demux/decode to GPU-accessible frames, CUDA processing, Vulkan display, and optional NVENC. Exit: benchmarked 1080p60 target path and measured 4K experiment, with copy/sync taxonomy.

## Phase 3 — Vulkan Observation Renderer
Goal: build the minimal renderer needed for image/world observation. Exit: renderer displays processed frames and simple WorldState overlays with validation clean.

## Phase 4 — AI Runtime
Goal: integrate TensorRT/CUDA-stream inference with measured preprocess and inference latency. Exit: model inference updates render overlay and metrics.

## Phase 5 — World State
Goal: make perception → WorldState → renderer a real runtime loop. Exit: structured state, observation, action, and transition logs.

## Phase 6 — Brain / Agent Loop
Goal: connect a mock/local/remote Brain provider to produce actions. Exit: Observation → Brain → Action → WorldTransition demo.

## Phase 7 — ROS 2 / Simulation / VLA Adapters
Goal: integrate external ecosystems without polluting Engine Core. Exit: adapter demos with measured boundaries and documented limitations.

## Phase 8 — AI World Engine Applications
Goal: compose runtime pieces into robotics, digital human, AD, XR, or physical-AI demos. Exit: public demo plus benchmark report.
