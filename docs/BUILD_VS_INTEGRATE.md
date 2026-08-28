# Build vs Integrate Matrix

| Component | Build | Integrate | Reuse | Decision | Reason |
| --- | --- | --- | --- | --- | --- |
| Runtime scheduler | Yes | No | Reference only | Build | Core learning/value area |
| GPU memory/resource abstraction | Yes | No | Reference only | Build | Core runtime ownership area |
| Video decode | No | Yes | NVIDIA/FFmpeg/GStreamer | Integrate | Codec implementation is not core |
| NVENC | No | Yes | NVIDIA SDK | Integrate | Encoding is an endpoint, not moat |
| CUDA kernels | Selectively | Yes | CV-CUDA where useful | Build small + integrate | Build enough to understand dataflow |
| Vulkan | No API build | Yes | Vulkan SDK | Integrate API, build thin renderer | Need explicit GPU control |
| TensorRT | No | Yes | NVIDIA TensorRT | Integrate | Inference engine is not core |
| Physics | No | Yes | PhysX/Bullet/simulator | Reuse/integrate | Full physics is out of scope |
| ROS 2 | No | Yes | rclcpp/RMW | Adapter only | Ecosystem bridge, not core |
| DDS | No | No | ROS 2 vendor | Reuse | Never implement DDS |
| VLA | No | Yes | OpenVLA/LeRobot/API | Adapter/application | Model training is out of scope |
| LLM | No | Yes | Remote/local providers | Adapter | Brain provider, not runtime core |
| World model | No training | Yes | External models later | Define runtime contract only | Core is WorldState/Observation/Action |
| Editor | No | No | Existing tools | Defer/delete | Not needed for MVP |
