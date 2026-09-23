# Zero-Copy Analysis

## Purpose

This document replaces promotional zero-copy language with precise copy and synchronization taxonomy.

## Taxonomy

| Category | Meaning | Acceptable wording |
| --- | --- | --- |
| True Zero Copy | Same allocation is shared without data movement and with explicit ownership/synchronization | "shared external memory" |
| Near Zero Copy | No host payload copy, but import/export, format conversion, mapping, or metadata work exists | "GPU-resident path" |
| Device-to-Device Copy | Data remains on GPU but is copied between allocations or layouts | "GPU copy" |
| Host-mediated Copy | Payload passes through CPU/system memory | "host copy" |
| Unavoidable Synchronization | Required fences, semaphores, queue waits, events, or layout transitions | "explicit synchronization" |

## Verified external facts as of 2026-08-28

- NVIDIA CUDA exposes APIs for importing external memory and external semaphores, and for asynchronous wait/signal on CUDA streams.
- Vulkan supports external memory with platform handle extensions such as POSIX file descriptors on Linux, and external semaphores for cross-API synchronization.
- NVIDIA NVDEC decodes compressed streams and places decoded YUV frames in video memory on discrete GPUs or system memory on Jetson-class platforms; post-processing can then be done with CUDA.
- TensorRT can enqueue inference on a CUDA stream, but some network features can cause synchronous behavior, so async behavior must be measured per engine.
- ROS 2 REP-2007/REP-2009 provide type adaptation and negotiation mechanisms; NITROS uses these ideas for hardware-accelerated graph negotiation, but process and transport boundaries must be measured before claiming zero-copy.

## Path-by-path assessment

| Path | Likely classification | Must verify |
| --- | --- | --- |
| NVDEC → CUDA | Near zero copy or device-to-device depending on SDK path and platform | Output memory type, mapping, pitch/format, hidden copies |
| CUDA → TensorRT | True zero copy if TensorRT bindings point at existing device buffers and lifetimes are correct | Binding ownership, stream ordering, dynamic shape behavior |
| TensorRT → CUDA/Vulkan overlay | True/near zero copy if output buffer is reused directly | Output format, postprocess kernels, synchronization |
| CUDA ↔ Vulkan | True zero copy for external memory sharing, but never zero synchronization | FD export/import, image layout, ownership transfer, semaphore waits |
| Engine ↔ ROS 2/NITROS | Near zero copy within compatible accelerated graphs; not assumed across all process/network boundaries | Type adaptation, negotiation, IPC, GPU handles, serialization |

## Forbidden claims

Do not write "0 copy", "GPU-only", "0 synchronization", or fixed latency numbers unless a benchmark report proves the exact path.
