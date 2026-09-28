# Zero / Low-Copy Analysis

## Claim policy

OmniWorld Engine uses "zero-copy" only when the same allocation is shared without payload movement. Most real GPU systems still require explicit synchronization, layout transitions, metadata handling, or device-to-device copies.

## Taxonomy

| Category | Definition | Example wording |
| --- | --- | --- |
| True Zero Copy | Same allocation shared between producers/consumers with no payload copy | shared external memory |
| Near Zero Copy | No host payload copy, but mapping/import/layout work exists | GPU-resident dataflow |
| Device-to-Device Copy | Payload copied on GPU between allocations or layouts | GPU copy |
| Host-mediated Copy | Payload moves through CPU/system memory | host copy |
| Unavoidable Synchronization | Required fences, semaphores, events, or barriers | explicit synchronization |

## External research checked on 2026-08-28

- [External Research] NVIDIA CUDA documentation exposes external memory and external semaphore interoperability APIs for cross-API sharing.
- [External Research] NVIDIA Video Codec SDK documentation states NVDEC places decoded YUV frames in video memory on GPUs or system memory on Jetson platforms, then CUDA post-processing may operate on those frames.
- [External Research] NVIDIA TensorRT documentation supports CUDA-stream execution, but some features may cause synchronous behavior.
- [External Research] ROS 2 REP-2007/REP-2009 define type adaptation and type negotiation; this is a basis for accelerator-friendly messaging but not proof of universal zero-copy across every process or transport.

## Required benchmark proof

Every pipeline edge must record memory residency, ownership, copy count, synchronization primitive, CPU waits, and p50/p95/p99 latency before being promoted from hypothesis to verified project knowledge.
