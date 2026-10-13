# CURRENT — 本周唯一焦点

> 只回答一个问题：**现在做什么。** 背景与全貌见 [`PLAN.md`](PLAN.md)。

**阶段 A 实施期：切片 01「真值心跳」✅ 完成；下一张 = 切片 02「观察生成」。**

## 当前状态

- 规划层全部冻结：L0 总体设计（契约 + 架构 + 阶段表）、L1 阶段 A 概要设计（模块 + 签名 + 模式出处）。
- **切片 01 已完成**（红→绿）：`core/types` + `core/rng` + `core/hash` + `world_state`（单作者，friend 通道）+ `truth_world`（确定性运动学）；ctest 2/2 绿，其中 `ow_slice01` 覆盖 5 组行为。详见 [L2](docs/design/stage-a-core/slices/01-truth-heartbeat.md)。
- 旧基线（重建对照）：Gate 1 三判据 1106.7 Hz / 适配器零核心改动 / ECE 分离 0.9617。

## 下一件事 = 领取切片 02

- **票据：[`.scratch/stage-a-core/issues/02-observation-generation.md`](.scratch/stage-a-core/issues/02-observation-generation.md)**（blocked by 01 —— 已解锁，可立即开始）
- 流程：新开一个 session，粘入这段话即可——

  > 实现 .scratch/stage-a-core/issues/02-observation-generation.md——先读 spec 与 docs/design/ 设计文档（含切片 01 的 L2 与既有代码），产出该切片的 L2 详细设计给我批准，批准后 TDD 实现。

- 切片链：01 ✅ 真值心跳 → 02 观察生成 → 03 感知接缝 → 04 裁决与信念账本 → 05 指标组 → 06 世界模型接缝 + Gate-1 对齐报告（06 出口 = 阶段 A 完成）。

## 出口条件（阶段 A）

Gate-1 对齐报告落 `benchmarks/regression/`：新实现三判据 vs `pre-rebuild` 基线并排，差异逐条解释；全量 ctest 绿 + 参考模式逐字节复现。

## 不碰

双模式框架（阶段 B）、ONNX 真模型（阶段 C）、Vulkan 渲染器（可选技能项目）、Action（契约预留）、付费大算力；**无批准的 L2 不写代码**（PLAN 规则 #6）。
