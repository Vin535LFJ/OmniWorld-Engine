# Project Sizing

Use size to choose process depth. The lifecycle is a control system, not a reason to delay coding.

## S — Experiment

Examples: CUDA kernel test, API smoke test, one-file timing probe.

Required:
- Goal
- Experiment
- Result
- Knowledge label

Optional:
- Benchmark note if performance is measured
- Review note if it affects future design

## M — Feature

Examples: GPU Frame abstraction, metrics module, logging module.

Required:
- Requirements
- Design
- Implementation
- Test
- Benchmark if performance-sensitive
- Review

Optional:
- ADR if a major trade-off is decided
- Contract if the feature crosses module boundaries

## L — Subproject

Examples: GPU Video Engine, Vulkan Renderer, AI Runtime.

Required:
- Project Init
- Requirements
- Research
- Architecture
- Module Design
- Contracts
- Implementation plan
- Testing strategy
- Benchmark strategy
- Review

Optional:
- Release plan
- Multiple ADRs
- Project-local knowledge folders

## XL — Major System

Examples: full OmniWorld Engine, end-to-end AI World demo.

Required:
- Complete lifecycle
- Architecture review gates
- Cross-project contracts
- Release/milestone plan
- Regression benchmarks
- Lessons learned and knowledge promotion

## Scaling Rule

If the task is small, use the smallest useful template. If a decision affects Core Architecture, data ownership, GPU synchronization, or public benchmarks, treat it as at least M and consider an ADR.
