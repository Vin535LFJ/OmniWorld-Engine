# OmniWorld Knowledge Base

This directory is the long-term learning and review system for OmniWorld Engine.

## Core goal

The Knowledge Base must be traceable, verifiable, and tied to the actual project. Do not bulk-generate knowledge notes from Codex or model memory alone.

## Knowledge loop

```text
Research Question → Source Research → Draft Knowledge → Human Review → Experiment / Source Verification → Project Knowledge → Review
```

Project learning should connect back to implementation:

```text
Knowledge ↔ runtime_probe ↔ Experiment ↔ Benchmark
```

## Source priority

Use sources in this order:

1. **Tier 1 — Official primary sources**: NVIDIA CUDA documentation, Khronos Vulkan/OpenGL/OpenCL documentation, Microsoft DirectX documentation, NVIDIA TensorRT and Video Codec SDK documentation, FFmpeg documentation, ROS 2 documentation, CARLA documentation, NVIDIA Isaac / Isaac ROS documentation, official GitHub repositories, and official technical blogs.
2. **Tier 2 — High-quality technical sources**: official samples, official GitHub examples, academic papers, top conference papers, high-quality engineering blogs, large open-source project source code, and high-quality tutorials.
3. **Tier 3 — AI Research**: Deep Research, DeepSeek, ChatGPT, Claude, Gemini, Perplexity, and similar tools may help organize material, explain concepts, compare approaches, find sources, generate exercises, summarize papers, and build learning structure. AI output is not Verified Knowledge by itself.
4. **Tier 4 — Project Experiment**: critical technical conclusions must ultimately be checked through OmniWorld experiments, benchmarks, or source verification.

## Source labels

Every note must use one or more labels:

- [Official]: supported by official documentation, official repositories, official samples, or official technical blogs.
- [Paper]: supported by academic papers or conference papers.
- [GitHub]: supported by source code or examples in a GitHub repository.
- [External Research]: supported by non-project external research material.
- [AI Assisted]: AI helped organize, explain, compare, or draft the note.
- [Inference]: reasoned from sources but not directly stated.
- [Hypothesis]: plausible but not verified.
- [Unverified]: source is missing, uncertain, possibly outdated, or version-dependent.
- [Experimental]: tested in an OmniWorld spike or experiment but not yet promoted.
- [Verified]: reproduced by a project benchmark, test, source check, or reviewed official source.
- [Project Knowledge]: validated knowledge that directly informs OmniWorld Engine development.

If a reliable source is not available, write `Source not yet verified` instead of inventing a link.

## Required note sections

Every Knowledge Note must include a `Sources` section with the following buckets, even if some are empty:

- Official documentation:
- GitHub:
- Paper:
- AI-assisted research:
- Local experiment:

Every important topic should begin with `knowledge/<topic>/research-questions.md` before detailed notes are promoted.

## Promotion model

Knowledge maturity follows this path:

```text
Raw → Research → Learning → Experimental → Verified → Project Knowledge
```

AI Research may only produce Draft Knowledge. It must pass human review and source or experiment verification before becoming Project Knowledge.

## External Research to Project Knowledge rule

Use this traceability chain:

```text
External Research → Knowledge Note → Experiment → Benchmark → Project Knowledge
```

Example:

```text
CUDA Stream documentation → CUDA Stream concept → two-stream overlap experiment → measured benchmark → OmniWorld async execution design
```

## Phase 1 scope

Phase 1 focuses only on knowledge required by current implementation and `runtime_probe` validation:

- C++: C++20 basics required by the project, RAII, smart pointers, error handling, Result / Status, struct / enum, header/source organization, basic CLI.
- CMake: targets, executable, library, `target_link_libraries`, compile features, tests, CMake Presets.
- Linux: processes, dynamic/shared libraries, environment, GPU driver, PCI/device basics, NVIDIA driver, Vulkan loader.
- GPU basics: GPU architecture basics, GPU device, compute capability, memory, VRAM, queue, execution, synchronization.
- CUDA: device properties, memory, Stream, Event, synchronization, timing.
- Vulkan: Instance, Physical Device, Device properties, Queue family, Memory heap, Validation Layer.

Do not expand Phase 1 into advanced topics unless current implementation has a real dependency.

## Research Queue

Keep advanced or premature topics in Research Queue instead of Core Architecture, including CUDA Graph, advanced kernel optimization, WMMA, Tensor Core programming, full Vulkan rendering, Render Graph implementation, Ray Tracing, TensorRT optimization, ROS 2, VLA, and World Model training.

## Topic structure

Important topics should use a learning-practice-review structure:

```text
01-concept.md
02-practice.md
03-experiment.md
04-benchmark.md
05-review.md
```

Review questions must check whether the author can explain the topic independently:

- Concept: What is it?
- Why: Why does OmniWorld need it?
- Implementation: How do we create or use it?
- Performance: What happens if it is used inefficiently?
- Debugging: How do we detect failures or stalls?
- Architecture: Where should ownership or boundaries live?
- Interview: Explain it against a related concept.

## Research records

Record research activity under `knowledge/research-log/` with date, topic, question, AI used, sources checked, conclusion, unresolved questions, and promotion status.

## Language rule

Write Knowledge Base notes primarily in Chinese. Keep technical terms in English with Chinese explanation when helpful, for example: `CUDA Stream（CUDA 流）用于组织异步 GPU operations。`
