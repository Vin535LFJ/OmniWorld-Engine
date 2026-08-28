# 30 / 60 / 90 Day Plan

## 0–30 Days — Runtime Probe Only

Focus: finish the GPU Runtime Foundation.

- Learn: C++20 project layout, CMake/Ninja, Linux GPU environment, CUDA device model, Vulkan physical-device model, metrics basics.
- Build: OmniWorld Runtime Probe.
- Include: CPU, GPU, CUDA, Vulkan, VRAM, driver, runtime configuration, logging, error reporting, metrics timestamps, basic benchmark command.
- Do not include: video decode, TensorRT, ROS 2, simulation, VLA, digital human, renderer architecture.
- Exit artifact: `runtime_probe` produces a reproducible capability report.

## 31–60 Days — GPU Video Pipeline

Focus: prove the first video-to-GPU path.

```text
Video
 ↓
Decode
 ↓
GPU Frame
 ↓
CUDA
 ↓
Measured Output / Renderer Bridge Stub
```

- Learn: FFmpeg demux, NVDEC concepts, GPU frame lifecycle, CUDA kernels, streams, events, buffer pools, frame timestamps.
- Build: video input prototype, frame abstraction, CUDA processing stage, CPU-vs-CUDA comparison, stage metrics.
- Benchmark: decode throughput, kernel latency, copy count, queue depth, frame drops, p50/p95/p99 latency.
- Exit artifact: measured GPU video processing path with clear copy/sync taxonomy.

## 61–90 Days — Video + AI + Rendering Integration Spike

Focus: integrate one measured perception path, not a full platform.

```text
Video
 ↓
GPU Frame
 ↓
TensorRT
 ↓
Detection
 ↓
Vulkan Overlay
```

- Learn: minimal Vulkan texture/display path, TensorRT runtime execution, preprocessing contracts, async stream boundaries.
- Build: renderer bridge, TensorRT detection spike, overlay rendering, end-to-end metrics.
- Benchmark: inference latency, FPS, VRAM, throughput, present latency, synchronization waits.
- Exit artifact: one demo showing video frames processed on GPU, inference results produced, and overlays rendered with measured latency.
