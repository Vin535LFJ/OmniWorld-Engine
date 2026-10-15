# 04: 裁决与信念账本

**What to build:** 关联判定（可替换策略，v1 = 最近位置 + 类别匹配）+ 信念账本：每张便签三分支留痕（匹配已有实体 / 铸新实体 / 拒绝+原因），裁决结果携带它裁决的内容；真值零写入。

**Blocked by:** 03（感知接缝）

**Status:** implementing — L2 已产出：[04-adjudication-belief-ledger.md](../../../docs/design/stage-a-core/slices/04-adjudication-belief-ledger.md)（预授权执行：切片 03–06 一次性授权，2026-10-14）

**流程：** L2 详细设计 → owner 批准 → TDD 实现 → problem-log → 走查。

- [ ] 三分支全部走通且原样留痕：Matched（挂到已有 id）/ NewEntity（runtime 铸新 id，永不复用）/ Rejected（原因留痕）
- [ ] BeliefEntry = Proposal + AdjudicationResult 一起存（架构评审洞见 #1"裁决结果应携带它裁决的内容"的落地）
- [ ] 关联策略可替换：测试替身（test double）换入换出零核心改动（策略模式验收）
- [ ] 真值零写入：整个流程中 WorldState 与真值字段不被任何便签触碰（结构性验证）
- [ ] v1 只实现一种策略（最近位置+类别），策略族后置（L1 风险 R9）
