# Codex Backlog

| ID | Title | Description | Dependencies | Priority | Complexity | Acceptance Criteria | Files Likely Affected | Benchmark | Definition of Done |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| E1-F1-T1 | C++20 build skeleton | Create minimal CMake/Ninja project | None | P0 | M | Configures and builds | `CMakeLists.txt`, `cmake/` | configure time | CI/local build passes |
| E1-F1-T2 | Version recorder | Emit toolchain/GPU versions | T1 | P0 | S | Machine report generated | `tools/`, `docs/TECH_STACK.md` | version report | Report committed |
| E1-F2-T1 | Metrics API design | Define timers/counters schema | T1 | P0 | M | Timer overhead measured | `engine/foundation/` | timer microbench | API documented |
| E2-F1-T1 | CUDA probe | Compile/run CUDA smoke test | T1 | P1 | M | CUDA device query passes | `engine/gpu/`, `apps/` | launch latency | Results recorded |
| E2-F2-T1 | Vulkan probe | Compile/run Vulkan validation smoke test | T1 | P1 | M | Validation clean | `engine/renderer/`, `apps/` | frame timing | Results recorded |
| E3-F1-T1 | CUDA/Vulkan interop spike | Test external memory/semaphore path | CUDA/Vulkan probes | P1 | L | Copy/sync classification written | `engine/gpu/`, `engine/renderer/` | semaphore wait latency | Gate 1 decided |
| E4-F1-T1 | NVDEC spike | Decode sample video to GPU-accessible frame | Build skeleton | P1 | L | Memory model documented | `engine/media/` | decode latency | Gate 2 decided |
| E5-F1-T1 | TensorRT spike | Run one model with device buffers | CUDA probe | P2 | L | Async behavior measured | `engine/inference/` | enqueue latency | Gate 3 decided |
| E6-F1-T1 | Minimal WorldState schema | Define state/observation/action | Metrics | P1 | M | Schema reviewed | `engine/world/`, `docs/world-model.md` | update latency later | API accepted |
| E7-F1-T1 | ROS 2 boundary study | Prototype adapter only after MVP-3 | WorldState | P3 | L | No core dependency introduced | `adapters/ros2/` | IPC latency | Gate 4 decided |
