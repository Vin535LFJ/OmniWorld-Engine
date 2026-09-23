# Risk Register

| ID | Risk | Impact | Probability | Mitigation | Gate |
| --- | --- | --- | --- | --- | --- |
| R1 | Scope expands into full engine/simulator/AD stack | Project stalls | High | Enforce adapter boundaries and MVP ladder | Every milestone |
| R2 | Zero-copy claims are inaccurate | Credibility loss | High | Use copy taxonomy and benchmark evidence | Gate 1/2/4 |
| R3 | CUDA/Vulkan interop unstable on target hardware | MVP delay | Medium | Fallback to staging/texture bridge | Gate 1 |
| R4 | NVDEC memory model blocks desired ownership | Extra copies | Medium | Compare SDK, FFmpeg, GStreamer, DeepStream, Vulkan Video | Gate 2 |
| R5 | TensorRT hidden synchronization | Latency jitter | Medium | Inspect engine features, plugins, dynamic shapes | Gate 3 |
| R6 | ROS 2/NITROS boundary over-promises process-wide GPU sharing | Misleading architecture | Medium | Keep ROS 2 adapter optional and measured | Gate 4 |
| R7 | Custom render graph consumes too much time | Core delayed | Medium | Limit renderer to observation/display until MVP-3 | Gate 5 |

## Decision gates

- Gate 1: CUDA/Vulkan interop continue/restructure/abandon.
- Gate 2: NVDEC path continue/restructure/choose integration alternative.
- Gate 3: TensorRT async path continue/restructure/simplify.
- Gate 4: ROS 2/NITROS adapter continue/restructure/defer.
- Gate 5: Render graph continue/reduce/reuse renderer.
