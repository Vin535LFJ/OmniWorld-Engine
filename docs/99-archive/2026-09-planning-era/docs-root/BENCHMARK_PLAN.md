# Benchmark Plan

## Performance number policy

Historical numbers such as `<6 ms`, `150+ FPS`, `<5% CPU`, `<0.05 ms`, `80% vs 3%`, and `45 ms vs 4 ms` are **unsupported illustrative targets** until reproduced by this repository with hardware, driver, OS, dataset, commit hash, and benchmark command recorded.

## Metrics

- Throughput: FPS, frames/s, inference/s, encode/s.
- Latency: capture→decode, decode→preprocess, preprocess→inference, inference→render, render→display, end-to-end.
- Jitter: p50, p95, p99, max.
- CPU: utilization, context switches, thread time, CPU waits.
- GPU: SM utilization, memory utilization, VRAM, copy engine, NVDEC, NVENC.
- Memory: Host→Device, Device→Device, Device→Host, allocation count, peak VRAM, buffer reuse.
- Synchronization: CPU waits, CUDA events, semaphore waits, queue stalls, pipeline bubbles.

## Required report fields

Every benchmark result must include OS, kernel, CPU, GPU, driver, CUDA, Vulkan SDK, TensorRT, Video Codec SDK/FFmpeg, compiler, build type, input media, resolution, model, command, commit, and raw result artifact path.

## Test pyramid

```text
Unit
  ↓
Component
  ↓
Integration
  ↓
GPU Integration
  ↓
End-to-End
  ↓
Performance Regression
```

Coverage must include resource lifetime, CUDA error paths, Vulkan validation, synchronization correctness, frame drops, backpressure, timestamp propagation, device loss, model failure, and adapter failure.
