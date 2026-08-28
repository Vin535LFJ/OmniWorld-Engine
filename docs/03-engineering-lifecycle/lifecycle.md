# OmniWorld Project Lifecycle

Use this lifecycle for L and XL work. S and M work may use the reduced process in `project-sizing.md`.

| Phase | Name | Input | Output | Entry Criteria | Exit Criteria | Owner | Required documents | Optional documents |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | Idea / Problem | Pain point, opportunity, bug, demo need | Problem statement | Problem is relevant to roadmap, exploration, or backlog | Problem is clear enough to discuss scope | Developer | Project Init for M/L/XL | Raw note |
| 1 | Requirements | Problem statement | Functional and non-functional requirements | Goal and non-goals are stated | Inputs, outputs, constraints, dependencies, success criteria are explicit | Developer | Requirements for M/L/XL | User stories |
| 2 | Research & Knowledge | Requirements, unknowns | Research notes and knowledge links | Unknowns are listed | Research Exit Criteria are answered or a Spike is required | Developer | Research note | Paper/GitHub/API notes |
| 3 | Feasibility / Spike | Research questions, risks | Spike/PoC result | Risk cannot be resolved by reading alone | Feasible/infeasible decision plus benchmark or evidence | Developer | Experiment note | Tiny prototype README |
| 4 | System Design | Requirements, research evidence | System design | Feasibility is plausible | Major dataflow, ownership, runtime boundary, and constraints are chosen | Developer | Architecture note for L/XL | Diagrams |
| 5 | Architecture Design | System design | Architecture decision or update | Major options are known | Trade-off is documented; ADR exists for major decisions | Developer | ADR for major choices | Architecture review note |
| 6 | Module Design | Architecture | Module responsibilities and boundaries | Architecture boundary is accepted | Modules have responsibilities, dependencies, and failure modes | Developer | Module design for M/L/XL | Sequence diagrams |
| 7 | Interface / Data Contract | Module design | Contracts and API sketch | Modules need to exchange data | Input, output, ownership, lifetime, sync, timestamp, error model, performance expectations are specified | Developer | Contract for cross-module boundaries | API draft |
| 8 | Implementation | Ready design and contracts | Code | Definition of Ready is met | Code builds, follows design, and does not bypass contracts | Developer | Task notes or PR description | Implementation note |
| 9 | Testing | Code, requirements | Test result | Feature is buildable | Unit/smoke/integration tests pass or limitations are documented | Developer | Test plan for M/L/XL | Test logs |
| 10 | Benchmark | Code, benchmark plan | Benchmark result | Performance claim or runtime path exists | Latency/FPS/VRAM/copy/sync metrics are recorded where relevant | Developer | Benchmark note | Raw benchmark logs |
| 11 | Review | Tests, benchmarks, diffs | Review decision | Evidence exists | Continue, fix, rollback, or promote decision is explicit | Developer | Review note | Weekly review |
| 12 | Documentation / Knowledge Promotion | Review result | Updated docs and knowledge | Result is useful beyond one task | Knowledge is labeled with trust level and linked to project/design/code/benchmark | Developer | Learning/research/architecture update | Lessons learned |
| 13 | Release / Milestone | Completed work | Tag, milestone, or next iteration plan | Definition of Done is met | Next action and owner are known | Developer | Release/milestone note for L/XL | Changelog |

## Definition of Ready

Formal implementation may start only when these are true:

- Problem, goal, non-goals, requirements, and constraints are written.
- Inputs and outputs are known.
- Dependencies and risks are identified.
- Required research questions have answers or a Spike is planned.
- The module boundary and data contract are clear enough to avoid rewrites.
- Benchmark target and success criteria are measurable.

## Definition of Done

A project or feature is done only when these are complete:

- Code builds and is integrated in the intended path.
- Required tests pass or limitations are documented.
- Benchmark results are recorded for performance-sensitive work.
- Documentation reflects actual behavior, not hopes.
- Knowledge notes are promoted with a trust label.
- Review decides whether to continue, fix, rollback, or release.

## Research Exit Criteria

Stop researching and start a Spike or implementation when you can answer:

1. What problem does this technology solve?
2. Why does it fit OmniWorld now or later?
3. What are its limits and failure modes?
4. What alternatives were considered?
5. What is the biggest risk?
6. What experiment or benchmark will validate it?

## Traceability Chain

```text
Research topic
  → knowledge note with trust label
  → project requirement/design
  → module contract
  → code path
  → test result
  → benchmark result
  → review / ADR / release decision
```

## Knowledge Trust Labels

Use one label at the top of notes that make technical claims:

- `[Raw]`
- `[External Research]`
- `[Inference]`
- `[Hypothesis]`
- `[Experimental]`
- `[Verified]`
- `[Project Knowledge]`
- `[Architecture Decision]`
