# CURRENT — 本周唯一焦点

> 只回答一个问题：**现在做什么。** 背景与全貌见 [`PLAN.md`](PLAN.md)。

**阶段 A 实施期：切片 01–05 ✅ 完成；最后一张 = 切片 06「世界模型接缝 + Gate-1 对齐报告」。**

## 当前状态

- 规划层全部冻结：L0 总体设计（契约 + 架构 + 阶段表）、L1 阶段 A 概要设计（模块 + 签名 + 模式出处）。
- **切片 01 已完成**（红→绿）：`core/types` + `core/rng` + `core/hash` + `world_state`（单作者，friend 通道）+ `truth_world`（确定性运动学）；ctest 2/2 绿，其中 `ow_slice01` 覆盖 5 组行为。详见 [L2](docs/design/stage-a-core/slices/01-truth-heartbeat.md)。
- **切片 02 已完成**（红→绿）：`observation`——有界视图（分辨率参数化、零写死常量）+ 三态可见性 + 帧元数据；ctest 3/3 绿（`ow_slice02` 7 组行为）。详见 [L2](docs/design/stage-a-core/slices/02-observation-generation.md)。
- **切片 03 已完成**（红=编译坑+桩断言 → 绿）：`core/proposal`（五元组 + schema 校验）+ `perception` 接缝（接口/自注册表/双 blob 检测器）；ctest 4/4 绿。详见 [L2](docs/design/stage-a-core/slices/03-perception-seam.md)。
- **切片 04 已完成**（红→绿）：`adjudication`（可替换关联策略：最近位置+容差+等距取先）+ `belief` 追加式账本（携裁决结果留痕）；ctest 5/5 绿。详见 [L2](docs/design/stage-a-core/slices/04-adjudication-belief-ledger.md)。
- **切片 05 已完成**（红→绿）：`metrics` 深模块（divergence / ECE / 身份稳定性，指标可插拔注册；ECE 判别分离 0 vs 0.125 精确值）；ctest 6/6 绿。详见 [L2](docs/design/stage-a-core/slices/05-metrics-suite.md)。
- 旧基线（重建对照）：Gate 1 三判据 1106.7 Hz / 适配器零核心改动 / ECE 分离 0.9617。

## 下一件事 = 领取切片 06（阶段 A 最后一片）

- **票据：[`.scratch/stage-a-core/issues/06-world-model-seam-gate1-alignment.md`](.scratch/stage-a-core/issues/06-world-model-seam-gate1-alignment.md)**（blocked by 05 —— 已解锁）
- 出口 = **Gate-1 对齐报告**落 `benchmarks/regression/`（新实现三判据 vs `pre-rebuild` 基线并排）→ 然后走 SOP-7 阶段收口（retro）。

- 切片链：01 ✅ 真值心跳 → 02 ✅ 观察生成 → 03 ✅ 感知接缝 → 04 ✅ 裁决与信念账本 → 05 ✅ 指标组 → 06 世界模型接缝 + Gate-1 对齐报告（06 出口 = 阶段 A 完成）。

## 出口条件（阶段 A）

Gate-1 对齐报告落 `benchmarks/regression/`：新实现三判据 vs `pre-rebuild` 基线并排，差异逐条解释；全量 ctest 绿 + 参考模式逐字节复现。

## 不碰

双模式框架（阶段 B）、ONNX 真模型（阶段 C）、Vulkan 渲染器（可选技能项目）、Action（契约预留）、付费大算力；**无批准的 L2 不写代码**（PLAN 规则 #6）。
