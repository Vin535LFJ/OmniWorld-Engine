# Third-Party Research Map

| Project/Tech | Why it exists | Why study it | Study this | Do not copy | Boundary with OmniWorld |
| --- | --- | --- | --- | --- | --- |
| ROS 2 | Robotics middleware and graph communication | Adapter ecosystem | rclcpp, executors, QoS, lifecycle | DDS implementation | Adapter only |
| Apollo | Autonomous-driving stack | Understand AD system boundaries | Module separation and dataflow | Full AD stack | External reference |
| Autoware | ROS-based AD platform | Robotics/AD integration patterns | ROS 2 packages and perception interfaces | Entire AD stack | External app/reference |
| CARLA | Autonomous-driving simulation | Synthetic sensors and scenarios | Sensor outputs, APIs, ROS bridge | Unreal simulator clone | Simulation adapter |
| Isaac Sim | NVIDIA simulation ecosystem | Robotics synthetic data and GPU workflows | Sensor/simulation integration | Full simulator | Simulation adapter |
| Isaac ROS | GPU-accelerated ROS packages | NVIDIA robotics acceleration patterns | NITROS graphs, GEM boundaries | Package ecosystem clone | ROS/GPU adapter reference |
| NITROS | Negotiated GPU-friendly ROS dataflow | Hardware-accelerated ROS boundary | Type negotiation/adaptation | Marketing zero-copy claims | Adapter experiment |
| OpenVLA | Open VLA robotics model | Brain provider pattern | Observation/action interface | Training stack | Brain adapter |
| LeRobot | Robotics ML datasets/models/tools | Policy learning ecosystem | Dataset/action abstractions | Training platform | Future app/reference |
| TensorRT | NVIDIA inference optimizer/runtime | Low-latency inference | C++ runtime, streams, memory bindings | Inference engine | AI Runtime dependency |
| CUDA | NVIDIA GPU compute API | Core compute/dataflow | streams, events, memory, graphs, interop | Whole framework | Core backend |
| Vulkan | Explicit graphics/compute API | Renderer and interop | memory, barriers, queues, external handles | Full engine/editor | Renderer backend |
| FFmpeg | Media demux/decode/encode framework | Practical media ingestion | demux, hwaccel integration | Codec implementation | Media dependency |
| CV-CUDA | CUDA image processing | GPU preprocessing | operators and tensor layouts | Entire library | Optional dependency |
| bgfx | Cross-API rendering abstraction | Design reference | backend abstraction trade-offs | Abstraction layer | Reference only |
| Filament | PBR renderer | Modern renderer architecture | materials, frame graph ideas | Full renderer | Reference only |
| Godot | Full game engine | Scope warning and architecture study | scene/resource concepts | Editor/engine clone | Not a target |
| O3DE | Large open 3D engine | Scope and engine architecture | asset/system modularity | AAA engine | Not a target |
| Unreal | Commercial-scale engine | Simulation/rendering reference | renderer/simulator boundaries | Engine clone | External ecosystem |
