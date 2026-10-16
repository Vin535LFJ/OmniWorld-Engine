# 01: 双模式 CLI + 工件 schema

**What to build:** `omniworld_loop --mode reference|performance`（默认 reference，未知值 fail-fast）；JSON 工件（run report）携带 `mode` 字段 + 完整环境元数据（OS/CPU/编译器/构建模式/commit），Markdown 报告层引用同一值（写入口单一）。参考模式行为**逐字节不变**——这是阶段 B 的常驻回归条件。

**Blocked by:** None（can start immediately）

**Status:** ready-for-agent

**流程：** 第一步产出本切片 L2（`docs/design/stage-b-dual-mode/slices/01-dual-mode-cli-artifact-schema.md`）→ owner 批准 → TDD 实现 → problem-log → 走查。

- [ ] `--mode reference|performance` 落地，默认 reference；未知模式立即报错不猜测
- [ ] JSON 工件含 `mode` + 环境元数据；Markdown 报告引用同一值（值来自单一写入口，不重复采集）
- [ ] 参考模式全量 ctest 绿 + 同 seed 双跑 `world_hash`/`ledger_hash` 逐位一致
- [ ] performance 模式 v1 = 与 reference 同路径 + 如实标注（不得假装 GPU——L1 风险 R-B2）；差异机制随 B2/B3 进入
- [ ] git diff 范围 = `apps/` + `loop` 工件写出面 + tests/CMake/文书（L1 §3 声明）
