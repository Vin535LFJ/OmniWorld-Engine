# Project Status

Effective 2026-09-21 (ADR-0007). Only one project may be `ACTIVE` as the main implementation route. Exploration is allowed only under the rules in `../03-engineering-lifecycle/exploration-rules.md`.

| Project / Topic | Status | Track | Why | Promotion condition |
| --- | --- | --- | --- | --- |
| Grounding Loop v0 (Phase 0) | ACTIVE | Engineering | The project's thesis must be proven: canonical world state shared by heterogeneous writers with measurable coherence — a demonstrated loop, not a document. | Gate 1 passed (`current-sprint.md` Week 4). |
| World Contract v0 | ACTIVE | Engineering | The loop's shared semantics (identity/time/ownership/confidence). Frozen until Phase-0 evidence exists. | Field-usage discipline: every field consumed by loop code; changes via ADR. |
| CUDA compute foundation | ACTIVE | Engineering | Runtime foundation (frames, timing, uploads/downloads) — used BY the loop, not the identity. | Continue as measured utilities (probe + memory discipline). |
| Replay + dynamics-model adapter (Phase 1) | NEXT | Engineering | First contact with world-model evaluation: prediction-vs-truth divergence on recorded rollouts. | Phase 0 exits Gate 1. |
| Public harness v0.1 (Phase 2) | LATER | Engineering | External use needs Gate 2 evidence, not optimism. | Gate 2: ≥1 genuine external signal. |
| Conflict-family & external physics (Phase 3) | LATER | Engineering | Only after MVP conflict data exists. | Phase-0/1 measurements show the ownership-only rule is insufficient. |
| WorldFoundry / cosmos-evaluator differential | EXPLORATION | Intelligence | Closest open landmarks; positioning must survive scrutiny by month 1–2. | Research note → ADR attachment. |
| Action-vocabulary mapping (WorldMark/WBench) | EXPLORATION | Intelligence | Phase-2 interop requirement; study only, no code yet. | Research note feeding Phase-1 Action contract. |
| OpenUSD one-way export | EXPLORATION | Intelligence | Future interop; explicitly NOT compliance/round-trip. | Only after a phase's exit criteria needs it. |
| ROS 2 bridge | BACKLOG | Intelligence | Decision stands: adapter role, 6-month minimum deferral. | Phase 3 re-evaluation with evidence. |
| VLA / LLM / Brain integration | BACKLOG | Intelligence | Agent contracts get their smallest Core interface in Phase 0; heavy integration is out. | Never via current phases; Gate-driven only. |
| Digital human, mobile/Jetson, distributed, editor, commercialization | BACKLOG | Intelligence | Out of scope (not-now.md). | — |

Historical note: the nine pre-2026-09 phases and five stub `projects/` were archived to `../99-archive/2026-09-planning-era/`; they no longer define status.
