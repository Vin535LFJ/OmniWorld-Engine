# Technology Stack and Version Policy

## Version policy definitions

- Pinned: exact version required for reproducibility.
- Minimum: oldest supported version.
- Tested: versions verified by CI or benchmark reports.
- Latest: allowed to float for research only.

| Component | Policy | Initial planning value | Notes |
| --- | --- | --- | --- |
| OS | Tested + Minimum | Ubuntu LTS Linux | Linux-first for POSIX FD and NVIDIA ecosystem |
| Kernel | Tested | Record per benchmark | Do not claim portability without testing |
| NVIDIA Driver | Minimum + Tested | Match CUDA/Video/TensorRT requirements | Record exact version |
| CUDA | Minimum + Tested | CUDA 12+ family | Needed for CUDA streams, graphs, external resources |
| Vulkan SDK | Minimum + Tested | Vulkan 1.3+ target | External memory/semaphore support must be probed |
| TensorRT | Minimum + Tested | TensorRT 10+ family | Pin per model benchmark |
| CV-CUDA | Tested | Optional in MVP-2 | Integrate only after simple CUDA kernels |
| Video Codec SDK | Tested | Current SDK on target machine | Compare with FFmpeg/GStreamer paths |
| FFmpeg | Pinned for benchmarks | Integration dependency | Demux and possible decode fallback |
| CMake | Minimum + Tested | 3.24+ candidate | Final version after skeleton |
| Ninja | Latest/Tested | Build tool | Record in CI |
| vcpkg | Pinned baseline | Dependency reproducibility | Commit baseline when adopted |
| ROS 2 | Adapter-tested | Humble or newer candidate | Not an Engine Core dependency |
| Compiler | Minimum + Tested | GCC/Clang C++20 | Record exact version |
| GPU architecture | Tested | NVIDIA discrete GPU first | Jetson semantics differ for memory |
