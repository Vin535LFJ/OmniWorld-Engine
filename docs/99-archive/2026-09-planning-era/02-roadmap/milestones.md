# Milestones

Each milestone must define: goal, scope, non-goals, dependencies, deliverables, acceptance criteria, benchmark, risks, and exit criteria.

## M0: Repository information architecture
Exit: audit, architecture, roadmap, knowledge base, templates, projects, benchmarks, and archive exist.

## M1: Build and capability probes
Exit: C++20/CMake skeleton builds; CUDA/Vulkan probes run; versions are recorded.

## M2: CUDA/Vulkan interop decision
Exit: external memory/semaphore prototype is measured and Gate 1 decides continue/restructure/fallback.

## M3: Video dataflow MVP
Exit: sample video path has measured decode, transform, display, copy count, and synchronization cost.

## M4: AI inference MVP
Exit: TensorRT model path is measured and integration boundaries are documented.

## M5: World runtime MVP
Exit: perception updates WorldState and renderer/Brain can consume observations/actions.
