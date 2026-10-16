# CURRENT — 本周唯一焦点

> 只回答一个问题：**现在做什么。** 背景与全貌见 [`PLAN.md`](PLAN.md)。

**阶段 B L1 已冻结；切片 B1–B2 ✅ 完成（ctest 9/9）；下一张 = B3「延迟计量 + 分位数下沉」。**

## 当前状态

- 规划层全部冻结：L0 总体设计（契约 + 架构 + 阶段表）、L1 阶段 A 概要设计（模块 + 签名 + 模式出处）。
- **切片 01 已完成**（红→绿）：`core/types` + `core/rng` + `core/hash` + `world_state`（单作者，friend 通道）+ `truth_world`（确定性运动学）；ctest 2/2 绿，其中 `ow_slice01` 覆盖 5 组行为。详见 [L2](docs/design/stage-a-core/slices/01-truth-heartbeat.md)。
- **切片 02 已完成**（红→绿）：`observation`——有界视图（分辨率参数化、零写死常量）+ 三态可见性 + 帧元数据；ctest 3/3 绿（`ow_slice02` 7 组行为）。详见 [L2](docs/design/stage-a-core/slices/02-observation-generation.md)。
- **切片 03 已完成**（红=编译坑+桩断言 → 绿）：`core/proposal`（五元组 + schema 校验）+ `perception` 接缝（接口/自注册表/双 blob 检测器）；ctest 4/4 绿。详见 [L2](docs/design/stage-a-core/slices/03-perception-seam.md)。
- **切片 04 已完成**（红→绿）：`adjudication`（可替换关联策略：最近位置+容差+等距取先）+ `belief` 追加式账本（携裁决结果留痕）；ctest 5/5 绿。详见 [L2](docs/design/stage-a-core/slices/04-adjudication-belief-ledger.md)。
- **切片 05 已完成**（红→绿）：`metrics` 深模块（divergence / ECE / 身份稳定性，指标可插拔注册；ECE 判别分离 0 vs 0.125 精确值）；ctest 6/6 绿。详见 [L2](docs/design/stage-a-core/slices/05-metrics-suite.md)。
- **切片 06 已完成**（红→绿）：`world_model` 第二接缝（zero_delta / linear_extrapolation 单文件模型）+ `loop` 驱动器（编排 + TickLog + 异常隔离实证）+ `apps/omniworld_loop`；ctest 7/7 绿；Release 38,094 Hz。详见 [L2](docs/design/stage-a-core/slices/06-world-model-seam-gate1-alignment.md) 与 [对齐报告](benchmarks/regression/2026-10-14-stage-a-gate1-alignment.md)。
- **阶段 B L1 已冻结**（2026-10-16，三轮共创）：`docs/design/stage-b-dual-mode/概要设计.md`；双模式 CLI/容差比较器/延迟计量 + F1 + F2 五切片就绪。
- 旧基线（重建对照）：Gate 1 三判据 1106.7 Hz / 适配器零核心改动 / ECE 分离 0.9617。

## 下一件事 = 领取切片 B5（阶段 B 最后一片）

- **票据**：[`.scratch/stage-b-dual-mode/issues/05-bounded-scene.md`](.scratch/stage-b-dual-mode/issues/03-latency-percentiles.md)（blocked by B1 —— 已解锁）
- 流程：新开 session 粘入——

  > 实现 .scratch/stage-b-dual-mode/issues/05-bounded-scene.md——先读 spec 与 L1（docs/design/stage-b-dual-mode/概要设计.md）、既有代码与容差比较器（src/tolerance/），产出 L2 详细设计给我批准，批准后 TDD 实现。

- **票据：[`.scratch/stage-b-dual-mode/issues/01-dual-mode-cli-artifact-schema.md`](.scratch/stage-b-dual-mode/issues/01-dual-mode-cli-artifact-schema.md)**（无阻塞，可立即开始）
- 流程：新开一个 session，粘入这段话即可——

  > 实现 .scratch/stage-b-dual-mode/issues/01-dual-mode-cli-artifact-schema.md——先读 spec（`.scratch/stage-b-dual-mode/spec.md`）与 L1（`docs/design/stage-b-dual-mode/概要设计.md`）及既有代码，产出该切片的 L2 详细设计给我批准，批准后 TDD 实现。

- 切片链：B1 ✅ 双模式 CLI+工件 schema → B2 ✅ 容差比较器+噪声 stub → **B3 延迟计量+分位数下沉** → B5 有界场景→ B5 有界场景（F2，出口 = 阶段 B 完成）。

- 切片链：01 ✅ 真值心跳 → 02 ✅ 观察生成 → 03 ✅ 感知接缝 → 04 ✅ 裁决与信念账本 → 05 ✅ 指标组 → 06 ✅ 世界模型接缝 + Gate-1 对齐报告（**阶段 A 完成**）→ **阶段 B：L1 已冻结，切片 B1–B5 就绪**。

## 出口条件（阶段 B，L0 §4）

参考模式逐字节不变（全量 ctest 绿）+ 性能模式容差自证（噪声 stub：窗内过、超窗红）。

## 不碰

ONNX 真模型（阶段 C）、Vulkan 渲染器（可选技能项目）、Action（契约预留）、付费大算力；sprint 外新想法进 `docs/17-research/research-queue.md`；**无批准的 L2 不写代码**（PLAN 规则 #6）。
