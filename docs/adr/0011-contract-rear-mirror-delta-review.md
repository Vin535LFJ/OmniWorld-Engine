# 契约后视镜 delta 审查结论（白纸 × v0.1 × 架构评审洞见）

Status: accepted（2026-10-12）——本 ADR 是 ADR-0010 第 4 条"后视镜 delta 审查"的执行记录；L0 §2 契约文本已按本结论定稿冻结。

对照物：白纸新契约（`docs/design/00-总体设计.md` §2，2026-10-12 owner 逐点裁决）× 旧 v0.1（tag `pre-rebuild` 的 PLAN §七）× 2026-10-09 架构评审 5 候选（`docs/17-research/2026-10-09-architecture-review.html`）。

## 结论

**A · 四处独立重合（双重验证）**：② 身份铸造与可替换关联 ≡ v0.1 语义①；③ 单一 tick 时钟域 ≡ 语义②；⑤ Proposal 五元组 ≡ v0.1 Proposal 字段（连命名一致）；⑦ 置信度单标量 / 不越权 / 只喂校准 ≡ 语义⑤。两套独立推导收敛于同一点——契约核心骨架的信心最高，这是白纸过程最值钱的产出。

**B · 三处收紧/简化**：④ 所有权由 per-entity `owner` 字段升为**字段类层面 + 结构性无接口**；⑥ 由"真值赢、留痕"简化为"**v1 无需仲裁**"（单一作者 ⇒ 真值侧冲突集为空）；⑧ 新增**分辨率属性化**（评审候选 #3 的 224 bug 从设计消失）+ 三态可见性显式化。评审洞见 #1"裁决结果应携带它裁决的内容"已内建（BeliefEntry = Proposal + 裁决结果一起存）。

**C · 五项缺口处置（owner 2026-10-12 批准）**：

1. **Tick 日志**（applied/rejected + per_stage_latency）→ **补为契约第 ⑨ 点**（FR3 延迟计量的契约载体）；
2. **帧元数据清单**（backend/device/format/dims）→ **并入 ⑧**（ADR-0009 要求性能模式标注后端）；
3. **Action**（智能体写路径）→ **显式延后**，契约留"未定，预留"（usage-driven）；
4. **state_version** → **声明"tick 即版本"**（不设独立计数器）；
5. **per-entity `source`** → **声明"来源由账本归属表达"**（不设字段）。

## Consequences

- L0 §2 为契约唯一权威文本；PLAN §七 已改为指针，v0.1 原文仅存 git 历史（tag `pre-rebuild` 前的 PLAN.md）。
- 评审候选 #2 / #4 / #5（metrics 抽 deep module、世界模型骨架 header seam、loop driver 合并）为**实现层课题**，归阶段 A 的 L1/L2 处置，不再挂起在仓库级。
