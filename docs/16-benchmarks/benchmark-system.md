# Benchmark System

## Directory layout

- `benchmarks/video/`: decode, encode, timestamps, frame drops.
- `benchmarks/cuda/`: kernels, streams, memory copies, events.
- `benchmarks/vulkan/`: frame time, validation, external resource behavior.
- `benchmarks/inference/`: TensorRT enqueue, preprocess, postprocess, batching.
- `benchmarks/memory/`: allocation, pooling, host/device/device copies.
- `benchmarks/end-to-end/`: video→AI→world→render latency.
- `benchmarks/regression/`: reproducible comparisons by commit.

## Required metrics

FPS, frame time, latency, throughput, CPU utilization, GPU utilization, VRAM, power, copy count, synchronization cost, p50, p95, p99, max.

## Rule

The benchmark report must explain why the system is fast or slow. "Because it uses GPU" is not an acceptable conclusion.
