# Third-Party Policy

Effective 2026-09-24. Governs all external code, models, weights, and datasets entering this repository. Trigger background: the component survey (`../17-research/2026-09-24-github-reuse-map.md`) found license contamination exactly where reuse is most tempting (perception models, trackers).

## License rules

**Allowlist (may enter the built/shipped path):** MIT, Apache-2.0, BSD (2/3-clause), Zlib, 0BSD.

**Denylist (may NOT enter the built/shipped path):**
- AGPL-3.0 (network-trigger copyleft; e.g., Ultralytics YOLO incl. weights and export scripts, FastSAM, boxmot)
- GPL-family (GPL-2.0/3.0, LGPL where dynamic-link isolation is not engineered)
- CC-BY-NC-* (non-commercial; e.g., Depth-Anything-V2 Base/Large/Giant — only its Small is Apache-2.0)
- No license file at all (all-rights-reserved; e.g., ssloy/tinyrenderer — reference-only reading, never copy code)

**Exception lane (isolated experimentation only):** a denylisted component may be used purely as a private experiment that never ships (not committed into product code, not required by the build), and only in a place where removal leaves zero residue. If in doubt: do not use it.

**Weights and datasets are licensed separately from code.** Check the per-model/per-dataset license even when the hosting repo is permissive (e.g., onnx/models is per-model). Record the result in the manifest.

## Vendoring rules

1. Vendored sources live under `third_party/<name>/` with the upstream LICENSE file copied verbatim; version pinned to a commit/tag and recorded in `third_party/MANIFEST.md` (repo URL, commit, license, why, owner).
2. Prefer system/package-level consumption (NuGet/vcpkg/FetchContent pinned to a tag) over copying source when the dependency is large (e.g., ONNX Runtime, Jolt).
3. No `git submodule` without an ADR.
4. A denylisted license found later in a dependency tree (transitive) must be removed within the current sprint.

## Determinism-sensitive dependencies

Anything in the replay/truth path (physics, rendering, numerics libraries) additionally requires a determinism audit before adoption: documented same-binary determinism contract, or our own seeded A/B runs proving bit-exact reproduction. Jolt Physics has such a contract in writing; Box2D v3 must run single-threaded; Bullet has no such statement.

## References

- Component survey and per-slot decisions: `../17-research/2026-09-24-github-reuse-map.md`
- Contract gate for integrations: `exploration-rules.md` / `lifecycle.md`
