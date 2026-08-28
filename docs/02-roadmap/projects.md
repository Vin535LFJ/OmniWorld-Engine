# Subproject Matrix

| Project | Purpose | Core Tech | Difficulty | Priority | Dependency |
| --- | --- | --- | --- | --- | --- |
| GPU Runtime Foundation | Common lifecycle, memory, scheduling, profiling | C++20, Linux, CUDA/Vulkan concepts | High | P0 | None |
| GPU Video Engine | Decode/process/display/encode GPU media pipeline | FFmpeg, NVDEC, CUDA, Vulkan, NVENC | High | P1 | Foundation |
| Vulkan Renderer | Observation/display and later world visualization | Vulkan, render graph, synchronization | High | P1 | Foundation/GPU memory |
| AI Runtime | Low-latency inference integration | TensorRT, CUDA streams, CV-CUDA | High | P1 | Foundation/GPU memory/video |
| World Runtime | Unified WorldState/Observation/Action contracts | Runtime data model, timestamps, state transitions | Medium | P1 | Foundation, renderer/AI optional |
| Brain Runtime | Model-agnostic decision provider | LLM/VLM/VLA adapters, local/remote APIs | Medium | P2 | World Runtime |
| ROS 2 Adapter | Robotics ecosystem bridge | rclcpp, REP-2007/2009, NITROS | High | P3 | World Runtime, benchmarks |
| Simulation Adapter | Synthetic world provider | CARLA, Isaac, PhysX, custom mocks | High | P3 | World Runtime |
| AI World Engine Apps | Portfolio demos | Composition of above | High | P3 | Proven modules |
