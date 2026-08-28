# Project Audit

## Audit scope

The repository audit inspected tracked files, top-level structure, hidden Git history, current Markdown/HTML research files, generated planning files from the previous commit, and the absence of source, build scripts, tests, examples, assets, benchmarks, and configuration.

## Current repository state

| Area | Current finding | Status |
| --- | --- | --- |
| README | Previously a long final-report style planning document | Rewritten as project homepage |
| Source code | No implementation code present | Current fact |
| Build system | No CMake/build scripts present | Current fact |
| Tests | No tests present | Current fact |
| Benchmarks | No executable benchmarks present before this restructure | Current fact |
| Research docs | Deep research report plus three discussion files and Chinese report | Archived as legacy knowledge |
| Previous generated docs | Broad planning docs and stub architecture docs | Archived as previous planning |
| Architecture decisions | Previous ADRs were generic and repetitive | Replaced with formal ADR location and template |
| Knowledge base | No structured learning/review system existed | Created |
| Project modules | No project-local knowledge structure existed | Created |

## Existing knowledge assets

| File/group | Main content | Long-term value | Issue |
| --- | --- | --- | --- |
| Legacy README | Project positioning, phase plan, zero-copy claims, stack choices | High | Too long for homepage; mixed facts, plans, and unsupported metrics |
| `deep-research-report.md` | Broad external project survey and roadmap ideas | High | Too broad; some time-sensitive claims need re-verification |
| `discussion_1.md` | Shift from renderer/game engine to GPU runtime | High | Informal conversation format |
| `discussion_2.md` | World Runtime and Brain adapter idea | High | Needs formal terminology |
| `discussion_3.md` | Strong GPU dataflow thesis and first sprint | Medium | Overstates zero-copy and performance claims |
| `Compute_Shader.htm` | Topic-specific external/HTML note | Medium | Should remain archived until distilled |
| Previous planning docs | Good first consolidation attempt | Medium | Too shallow for requested knowledge architecture |

## Repetition and overlap

- Multiple files repeat the same identity: GPU + Real-Time + AI + World Systems Engineer.
- Multiple files repeat the same anti-goals: not Unreal, not Apollo, not CARLA, not a VLA model.
- Multiple files repeat similar phased plans without a single canonical roadmap.
- Zero-copy and performance narratives appear in several places without verified benchmark evidence.

## Conflicts found

| Conflict | Resolution |
| --- | --- |
| Five-stage single project vs one main project plus subprojects | Use one monorepo with project folders and shared docs/knowledge/benchmarks |
| Renderer-centric vs runtime-centric identity | Runtime-centric; renderer is an observation/display module |
| ROS 2 as integrated pipeline vs adapter | ROS 2 is an adapter, never Engine Core |
| VLA/world model as project center vs provider | VLA/world models are Brain/Model providers, not core implementation targets |
| Zero-copy marketing vs measured engineering | Use copy taxonomy and benchmark gates |

## Outdated or unsafe content

- Any fixed performance number is unsafe unless reproduced with repo benchmarks.
- Any claim of `0 synchronization` is deprecated because explicit cross-API synchronization is required.
- Any implication that the first milestone includes ROS 2, VLA, simulation, TensorRT, and Vulkan together is too broad.

## What should be archived

All historical research, informal discussions, previous planning docs, and one-off HTML notes are preserved in `docs/99-archive/2026-08-legacy-planning/` for traceability.

## What becomes formal docs

- Project identity and current status: `README.md`, `docs/00-overview/vision.md`.
- System architecture and boundaries: `docs/01-architecture/`.
- Roadmap and subproject matrix: `docs/02-roadmap/`.
- Research map: `docs/17-research/`.
- Learning system: `knowledge/`, `templates/`, and project-local `knowledge/` folders.
