# Benchmark Report: runtime_probe Baseline (2026-09-18)

First recorded run of the Phase 1 `runtime_probe` executable. This is the capability/timing baseline for the GPU Runtime Foundation project.

## Required Metadata

| Field | Value |
| --- | --- |
| OS | Windows 11 (build 26200) |
| Kernel / kernel version | n/a (Windows; build 26200) |
| CPU | Intel Core i9-14900HX, 24 physical cores / 32 logical processors |
| GPU | NVIDIA GeForce RTX 4060 Laptop GPU (compute capability 8.9, 24 SMs, 8 GiB VRAM) |
| Driver | NVIDIA 560.94 (Vulkan raw 0x8c178000) |
| CUDA | Toolkit 12.6.20, driver API 12.6, runtime 12.6 |
| Vulkan SDK | Not installed; probes via system loader `C:\WINDOWS\SYSTEM32\vulkan-1.dll` (instance API 1.3.280) |
| TensorRT | Not installed (not required by this probe) |
| Video Codec SDK / FFmpeg | n/a for this probe |
| Compiler | MSVC 19.29.30159.0 (VS2019), C++20 (`/std:c++latest`), `/O2 /DNDEBUG` |
| Build type | Release (CMake + Ninja 1.10.2) |
| CMake | 3.20.0-rc1 |
| Dataset / media / model | n/a (capability probe, 64 MiB pinned host buffer for bandwidth measurement) |
| Command | see Commands below |
| Commit hash | 49922ea3bb61befdaa5eed2d5d8ac6f789c8ffe5 + uncommitted `runtime_probe` additions (first build of this target) |
| Raw artifact path | `benchmarks/regression/artifacts/2026-09-18-runtime-probe-baseline/` |

## Commands

```
vcvars64.bat
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
build\runtime_probe.exe --text
build\runtime_probe.exe --json
ctest --test-dir build --output-on-failure
```

## Key Results

- Exit code 0 (cpu/cuda/vulkan all ok); `ctest` smoke test passed (1/1).

### CUDA (RTX 4060 Laptop)

| Metric | Value |
| --- | --- |
| Compute capability | 8.9 (24 SMs) |
| Core / memory clock | 1890 MHz / 8001 MHz, 128-bit bus |
| L2 cache | 32.0 MiB |
| Global memory | 8.00 GiB total, 6.93 GiB free at probe time |
| H2D copy p50 | 5.287 ms (12.69 GiB/s), best 12.87 GiB/s |
| D2H copy p50 | 5.127 ms (13.09 GiB/s), best 13.10 GiB/s |
| Bandwidth method | 64 MiB pinned host buffer, 2 warmup + 5 samples per direction, cudaEvent timing, p50/best reported |

### Vulkan (via system loader, no SDK)

| Metric | Value |
| --- | --- |
| Physical devices | 1 (discrete GPU, API 1.3.280) |
| Driver | 560.94.0 (NVIDIA 10-bit/8-bit scheme) |
| Queue families | 6 (graphics x16, transfer x2, compute x8, transfer x1 x3) |
| Memory types / heaps | 5 types, 2 heaps |
| Heap 0 | 7.77 GiB device-local (VRAM minus WDDM reserve) |
| Heap 1 | 15.90 GiB host (Windows shared GPU memory = RAM/2) |
| Validation layers | absent (no Vulkan SDK installed) |
| Instance layers | 1 (VK_LAYER_NV_optimus) |

### Probe timing (steady_clock, Release build)

| Phase | Duration |
| --- | --- |
| CPU probe | 0.122 ms |
| CUDA probe | 184.266 ms (dominated by context init + 12 memcpys) |
| Vulkan probe | 221.818 ms (instance creation + enumeration) |
| Total | 406.209 ms |

## Environment Notes and Deviations

1. **Windows vs Linux target.** ADR-0003/ADR-0008 target Linux as the runtime OS, but the current development machine is Windows 11. The probe is written cross-platform: the CUDA and Vulkan paths are platform-neutral (runtime API / dynamic loader), and `cpu_probe.cpp` has a Linux branch (`/proc`, `uname`, `sysconf`) that is not yet exercised. Linux validation is pending until a Linux GPU box is available; treat the Linux branch as untested.
2. **No Vulkan SDK.** Physical-device probing loads `vulkan-1.dll` / `libvulkan.so.1` dynamically and declares the minimal ABI structs inline, so the probe runs without the SDK. Validation-layer reporting shows "absent" because no SDK is installed, not because the driver lacks layers.
3. **Struct ABI verified empirically.** `VkPhysicalDeviceMemoryProperties` field order was confirmed against live driver output (typeCount, memoryTypes[32], heapCount, memoryHeaps[16]); the first attempt with both counts leading produced invalid heap data (heapCount=0), fixed and verified against heap sizes (7.77 GiB + 15.90 GiB).
4. **`memory_type_0` has property flags 0x00.** Reported verbatim by the 560.94 driver on WDDM; recorded as-is.

## Next Steps

- Repeat the probe on Linux to validate the Linux CPU branch and driver discovery.
- Add repeated-run harness (p50/p95/p99 over N probe invocations) for probe startup and enumeration latency.
- Add Vulkan timestamp-query readiness check once the GPU Frame contract work begins.
