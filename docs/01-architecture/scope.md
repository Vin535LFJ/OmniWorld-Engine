# Scope

## In Scope

- C++20/Linux-first runtime foundation.
- GPU memory/resource lifetime abstractions.
- CUDA/Vulkan synchronization and interop experiments.
- Video ingest/decode/encode integration through external libraries/SDKs.
- CUDA/CV-CUDA image processing and TensorRT inference integration.
- Minimal Vulkan renderer for observation/display and later world visualization.
- WorldState, Observation, Action, and WorldTransition contracts.
- Brain/model adapters for local or remote LLM/VLM/VLA providers.
- ROS 2/NITROS adapters after core runtime proof.
- Benchmark, profiling, learning, and review systems.

## Out of Scope

- Training large foundation models, world models, VLA models, or LLMs.
- Reimplementing DDS, ROS 2, FFmpeg codecs, TensorRT, Vulkan, CUDA, or physics engines.
- Replacing Unreal Engine, CARLA, Isaac Sim, Apollo, or Autoware.
- Building a commercial game editor, full asset pipeline, scripting language, animation system, multiplayer system, or general-purpose game engine.
- Making unverified zero-copy or latency claims.

## Expansion rule

A new module may enter core only if it directly improves measurable GPU dataflow/runtime capability and cannot be handled cleanly as an adapter.
