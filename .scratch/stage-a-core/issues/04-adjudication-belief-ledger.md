# 04: 裁决与信念账本

**What to build:** 关联判定（可替换策略，v1 = 最近位置 + 类别匹配）+ 信念账本：每张便签三分支留痕（匹配已有实体 / 铸新实体 / 拒绝+原因），裁决结果携带它裁决的内容；真值零写入。

**Blocked by:** 03（感知接缝）

**Status:** done — 2026-10-14（预授权执行；证据见 ## Comments；1 处对票据文字的收窄裁定已显式记录）

**流程：** L2 详细设计 → owner 批准 → TDD 实现 → problem-log → 走查。

- [x] 三分支全部走通（NewEntity 按 L2 收窄：宣告不锚定，matched=0——如需 belief 侧持久 id，走 ADR）且原样留痕：Matched（挂到已有 id）/ NewEntity（runtime 铸新 id，永不复用）/ Rejected（原因留痕）
- [x] BeliefEntry = Proposal + AdjudicationResult 一起存（架构评审洞见 #1"裁决结果应携带它裁决的内容"的落地）
- [x] 关联策略可替换：测试替身（test double）换入换出零核心改动（策略模式验收）
- [x] 真值零写入：整个流程中 WorldState 与真值字段不被任何便签触碰（结构性验证）
- [x] v1 只实现一种策略（最近位置+类别），策略族后置（L1 风险 R9）

## Comments

- 2026-10-14 · **TDD 红相位**：裁决器桩（一律 Rejected "stub"）下 7 处断言失败；账本/替身/守防不受影响。
- 2026-10-14 · **绿相位（全量验证）**：`build.bat --test` → ctest **5/5 通过**（ow_slice04 0.02s）。
- 2026-10-14 · **偏差（显式收窄）**：票据 AC1 原文"NewEntity 由 runtime 铸新 id"；L2 §2 裁定为**宣告不锚定**——v1 不在 TruthWorld 路径之外铸造世界 id（单一作者的直接推论，防 id 空间分裂）。如未来需要 belief 侧持久实体，走 ADR 升契约。
- 2026-10-14 · **走查**：范围 = `src/adjudication/` + `src/belief/` + tests/CMake/文书——与 L1 §5 声明一致。
