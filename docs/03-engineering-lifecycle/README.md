# Engineering Lifecycle

This directory defines how OmniWorld Engine moves from idea to implementation without turning planning into procrastination.

## Lifecycle Flow

```text
IDEA → REQUIREMENTS → RESEARCH → KNOWLEDGE → FEASIBILITY → DESIGN → ARCHITECTURE
→ MODULE / CONTRACT → IMPLEMENTATION → TESTING → BENCHMARK → REVIEW
→ DOCUMENTATION → KNOWLEDGE PROMOTION → RELEASE → NEXT ITERATION
```

## Core Rules

1. One `ACTIVE` project remains the main engineering route.
2. Engineering Exploration and Intelligence Exploration are allowed, but they cannot change Core without review.
3. Unknown technology must go through Research → Spike/PoC → Benchmark → Architecture Review → ADR before Core adoption.
4. A project without Definition of Ready cannot enter formal implementation.
5. A project without tests, benchmark, documentation, knowledge update, and review is not done.
6. Document only to the level required by project size.

## Documents

- [`lifecycle.md`](lifecycle.md): phase-by-phase lifecycle contract.
- [`project-sizing.md`](project-sizing.md): how much process each task needs.
- [`exploration-rules.md`](exploration-rules.md): controlled exploration for ROS 2, VLA, World Model, Agent, and other uncertain technologies.

## Architecture Audit — 2026-08-28

- Documentation duplication: lifecycle rules are centralized here; roadmap documents remain focused on phase priority.
- Complexity check: small tasks use `project-sizing.md` and do not need full lifecycle documents.
- Boundary check: Project, Knowledge, Research, Benchmark, and ADR artifacts have separate roles.
- Track check: Engineering is the active build route; Intelligence is bounded Exploration.
- Exploration check: ROS 2, VLA, World Model, and Agent are allowed as Tiny Prototypes only, not Core work.
- Active-project check: only GPU Runtime Foundation is ACTIVE.
- Today check: `CURRENT.md` answers what to learn, build, research, avoid, and finish.
- Coding gate check: Definition of Ready blocks formal implementation until problem, requirements, risks, contract, and benchmark target are clear.
- Completion gate check: Definition of Done requires code, tests, benchmark, documentation, knowledge, and review.
