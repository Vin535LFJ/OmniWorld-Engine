# World State Architecture — World Contract v0

WorldState is the runtime-owned, canonical, versioned representation of one world. Heterogeneous components (perception models, world models, simulators, renderers, agents) interact with it **only** through the contracts in this document. Nothing else may become code.

## Design rule (usage-driven, additive-only)

1. Every field in v0 exists because Grounding Loop v0 code (current sprint, Weeks 2–4) reads or writes it. A field with no consumer is deleted, not deferred.
2. v0 never adds semantics; later versions may add **fields** but never re-interpret existing ones. Any semantic change = new contract version + ADR.
3. WorldState is not a trained world model. Foundation models and VLA systems are Brain/model providers. (Unchanged from previous revision.)

## Primitives

| Primitive | Meaning |
| --- | --- |
| World | Runtime-owned state space that can be observed, updated, rendered, and acted on. |
| Entity | One object in the world with a stable identity assigned by the runtime. |
| Observation | A bounded view of the World for one consumer, captured at one tick. |
| Proposal | A write attempt by a non-owner component, carrying confidence, adjudicated by the runtime. |
| WorldTransition | The measured record of one state update: what changed, why, and what it cost. |
| Action | A validated request to request a change (targets an entity; issued by an agent component). |

## Fields (v0)

**Entity** — one row per object:

| Field | Type | Semantics |
| --- | --- | --- |
| `entity_id` | u64 | Minted by the runtime, never reused, never guessed by adapters. |
| `class_tag` | string (free-text label) | Class label; conventions ("car", "chair") live in adapters, not in Core. |
| `transform` | pos(f32×3) + quat(f32×4) | SE(3) in the single world frame. |
| `velocity` | linear f32×3 (+ angular later, deferred) | Ground-truth worlds own kinematics. |
| `owner` | component id | Exactly one owner per entity; owners may write `transform`/`velocity` directly. |
| `source` | `ground_truth` \| `belief` | Truth-layer entities are simulation facts; belief-layer entities are model claims. |
| `updated_at` | tick | Last modification tick. |

**Observation** — carries the memory-contract buffer metadata (backend, device, format, dimensions, pitch/stride, lifetime, sync state, timestamp) inherited from `memory.md`, plus:

| Field | Semantics |
| --- | --- |
| `tick` | Capture tick in the runtime's monotonic tick domain. |
| `view_transform` | The observer's pose (camera) in the world frame. The ONLY place a second frame exists in v0. |
| `sensor_kind` | `rgb` in v0. Depth/segmentation buffers are adapter outputs, not core sensor kinds. |

**Proposal** — the universal write path for non-owners:

| Field | Semantics |
| --- | --- |
| `producer` | Component id (perception adapter, world model, ...). |
| `tick_observed` | Tick of the Observation the proposal is derived from. |
| `target` | `entity_id` (update) or `new_entity` (introduction). Final identity is adjudicated by the runtime's association rule below — the producer's suggestion is evidence, not decision. |
| `fields` | Proposed values (class_tag and/or transform and/or velocity). |
| `confidence` | f32 in [0,1], as supplied by the producer. Calibration issues surface as metric data, never as silent overwrites. |

**WorldTransition** — logged per tick:

| Field | Semantics |
| --- | --- |
| `tick`, `state_version` | Versioned world evolution; version increments per committed tick. |
| `applied`, `rejected` | Proposals applied vs rejected (rejection carries a reason code). |
| `per_stage_latency` | Timing decomposition per loop stage (the frame-budget data). |

**Action** — `target entity_id`, `action_type` + payload (schema is the agent's contract), `issued_by`, `tick`. Validation = schema + ownership of the actuator channel; semantics belong to the agent/adapter layer.

## Semantics contracts (what each word MEANS)

- **Identity.** The runtime mints `entity_id`; only it resolves "same entity?". v0 association rule: nearest transform with class-match above a fixed threshold — deliberately trivial, replaceable in v0.1 (tracking-aware association is a research-queue topic, not a promise).
- **Time.** One monotonic tick domain owned by the runtime. Wall-clock is recorded for diagnostics but never used for causality. Every read/write/Observation carries its tick; `tick_observed` on every Proposal is how perception staleness becomes measurable.
- **Ownership.** The truth world owns kinematics; perception/world models write Proposals into the belief layer only. A snapshot for Metrics is therefore `(truth entities, belief entities)` pairs joined by `entity_id` — that join is where the coherence metrics live.
- **Conflict resolution (v0).** Ownership rule only: truth wins, beliefs are recorded. `timestamp-wins` and `confidence-weighted` are Exploration topics with named entry criteria in MASTER-ROADMAP Phase 3.
- **Uncertainty.** A single `confidence` float per proposal. Field-level probability distributions are deferred (multiple-representation and uncertainty propagation are explicitly non-goals of v0).
- **Coordinate frames.** One world frame. Everything else (`view_transform` of an Observation) is scoped to the Observation and composition happens inside the capturing adapter.
- **Partial observation.** Observations carry explicit view metadata; the runtime never assumes unseen regions are unchanged versus unknown. v0 runs single-view; occlusion semantics arrive with multi-view in v0.1 research.

## What v0 explicitly does NOT contain

Multi-frame trees (tf2-style), distributed state sharing, DDS/ROS 2 compatibility beyond adapter possibility, probabilistic state distributions, semantic hierarchy/relationships, generative representations coexisting with geometric ones, OpenUSD export (exploration), renderer-for-display (v0 is offscreen), multiple simultaneous truth worlds, networking.

## Metric hooks (why each field exists, not decoration)

| Metric (Grounding Loop v0) | Required contract pieces |
| --- | --- |
| Identity stability (cross-frame association accuracy) | `entity_id`, `target` adjudication, `updated_at` |
| Transform error (belief vs truth) | belief/truth `source` join on one id namespace |
| Temporal coherence / staleness | `tick`, `tick_observed`, `state_version` |
| Frame budget decomposition | `per_stage_latency` in every WorldTransition |
| Metric discriminability (Gate 1c) | `confidence` present and honored by the adjudication path |

History: supersedes the first one-page version (kept in git history). Direction and evidence: [`ADR-0007`](decisions/ADR-0007-project-direction.md), [`../../17-research/2026-09-21-direction-decision.md`](../17-research/2026-09-21-direction-decision.md).
