# synchronization

See the consolidated architecture in `docs/ARCHITECTURE.md`. This document owns the detailed design for the synchronization area and must remain benchmark-driven.

## Responsibilities

- Define minimal contracts before implementation.
- Separate Engine Core ownership from adapter integration.
- Mark claims as verified, target, estimated, illustrative, or unsupported.
- Add decision gates before expanding scope.

## MVP constraints

- Prefer the smallest experiment that proves data ownership, synchronization, and latency semantics.
- Do not add ROS 2, VLA, physics, editor, scripting, or full asset pipeline dependencies to Engine Core.
