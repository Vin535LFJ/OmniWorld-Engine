# Spec — 阶段 B「双模式框架」

Status: ready-for-agent

> 来源：**L1 已冻结**（`docs/design/stage-b-dual-mode/概要设计.md`，owner 三轮共创 2026-10-16）+ 弹药简报（`docs/17-research/2026-10-15-stage-b-l1-briefing.md`，24 条已确认事实）+ ADR-0009 / L0 §2 契约⑨ / 对齐报告 Findings（F1/F2）。工具无关：任何 AI 按本 spec + L1 + L0 可领任务。

## Problem Statement

引擎的可复现性承诺只实现了一半：参考模式（纯 CPU、逐字节）已在阶段 A 落地并经 Gate-1 对齐报告验证；而 ADR-0009 的第二条承诺——**性能模式**（固定 seed + 固定输入 → 容差窗口 + 延迟预算达标）——在代码中完全不存在（pre-rebuild 时它停在契约与票据层，全仓 grep 零匹配；旧 CLI 的 `--mode` 只区分 phase0/phase1）。同时对齐报告排入两项修补：**F1** 聚合 ECE 被高产低信息的世界模型条目淹没（9600/9746），需要按 producer 分解；**F2** 无墙常速真值世界使实体数十 tick 内飘出观察窗口，观察后段枯竭。阶段 B 出口判据（L0 §4 原句）：**参考模式逐字节不变（全量 ctest 绿）+ 性能模式容差自证（噪声 stub）**。

## Solution

按 L1 §5 五片垂直推进：B1 双模式 CLI 与工件 schema → B2 容差比较器 + 噪声 stub 自证 → B3 延迟计量与分位数下沉 → B4 per-producer ECE（F1）→ B5 有界场景（F2）。每片走「L2 → owner 批准 → TDD → 走查 → 收口」。

## User Stories

1. As the engine owner, I want a `--mode reference|performance` CLI switch defaulting to reference, so that the two reproducibility contracts are selectable and the default stays the byte-exact oracle.
2. As a portfolio viewer, I want every JSON artifact to carry the mode plus full environment metadata, so that performance numbers are self-labeling and verifiable (ADR-0009).
3. As the engine owner, I want a tolerance comparator that takes two metric maps and per-metric windows, so that tolerance judgment is programmatic and testable as pure data.
4. As the engine owner, I want a noise-stub test double with a parameterized amplitude, so that the tolerance mechanism self-proves in both directions (within window → pass, beyond → fail) without real models.
5. As the engine owner, I want per-stage latency raw microsecond samples kept and p50/p95 computed inside the library, so that budgets can be asserted programmatically while raw evidence stays intact.
6. As the engine owner, I want a latency-budget assertion mount point carrying an initial budget from the alignment run, so that "delay budget met" becomes a checkable condition.
7. As the engine owner, I want ECE reported both aggregated and per-producer, so that a high-volume low-information producer can no longer drown the signal (problem-log 005).
8. As the engine owner, I want to re-run the alignment scenario and honestly analyze whether in-loop discrimination appears with per-producer ECE, so that criterion (c)'s PARTIAL has a data-backed trajectory.
9. As the engine owner, I want a `bounds(none|wall)` scene policy injected at construction, so that the no-wall alignment baseline stays reproducible while wall bounces provide learnable structured prediction errors.
10. As the engine owner, I want wall bounces deterministic with clamp/invariant tests, so that the recorded "comment says clamp but no clamp" deviation is fixed rather than inherited.
11. As the engine owner, I want boundary events counted in the TickLog, so that F1's ECE grouping can separate scene-event error from model error.

## Implementation Decisions

- 全部接口签名与模块边界见 L1 §3/§4（`RunMode`/`BoundsPolicy` 无默认值——默认只许存在于 CLI 解析层；`WorldState` 构造注入 bounds；`TickLog.boundary_events` 为契约⑨ 追加字段，L0 §2 注释已同步）。
- 容差比较器为 loop 私有子件（`src/tolerance/`），纯数据进、判定出；噪声 stub 是**测试替身**（先例：slice04 `AlwaysReject`、slice06 `ThrowingPerception`），不进 `src/`。
- per-producer ECE 在 Metrics 深模块内实现：report 键 = 聚合键 `"ece"` + 分解键 `"ece:<producer>"`（平铺、排序确定）。
- F2：`bounds` 策略经 RunConfig 由 LoopDriver 构造时注入 WorldState；wall = 镜像反射 + 速度取反；常数边距 0.05→0.02（与默认 footprint 对齐，文档写明耦合）；Entity `extent` 字段**延后**。
- 参考模式逐字节不变是每片的常驻回归条件；墙钟永不进哈希。

## Testing Decisions

- 行为断言 + 确定性断言**成对**（problem-log 002）；聚合指标必须附按来源分解断言（反模式 #13）。
- 容差自证两向实跑（窗内过、超窗红）；wall 的"单轴单 tick 恰一次反射"不变式测试。
- 分位数口径与 pre-rebuild runner 一致（排序后线性插值），用已知样本集验证插值行为。
- 参考模式回归：全量 ctest 绿 + 同 seed 双跑哈希一致（每片必跑）。

## Out of Scope

真实 GPU 组件（阶段 C 随真模型进；性能模式 v1 不得假装 GPU——R-B2）；ONNX Runtime；Entity `extent` 字段；wrap 回绕策略（Round 2 已排除并记录理由）；WorldMark 对齐与评测工具产品化（期权分支）。

## Further Notes

- 切片的依赖边、出口判据见 L1 §5；每片 L2 落 `docs/design/stage-b-dual-mode/slices/NN-*.md`。
- 实现中问题随遇随记 `docs/design/problem-log.md`；STAR 素材阶段末进 `docs/interview-narrative.md`（W02 步骤 4）。
