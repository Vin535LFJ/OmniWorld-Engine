# CURRENT — 本周唯一焦点

> 只回答一个问题：**现在做什么。** 背景与全貌见 [`PLAN.md`](PLAN.md)。

**阶段 C L1 已冻结（2026-10-16，Round 1+2 六项裁决）；spec + 5 张切片票已发布；下一张 = C1「依赖隔离 + provider 框架」。**

## 当前状态

- **阶段 A 完成**（6 切片 + Gate-1 对齐报告：[`benchmarks/regression/2026-10-14-stage-a-gate1-alignment.md`](benchmarks/regression/2026-10-14-stage-a-gate1-alignment.md)，判据 (c) PARTIAL 如实标注）。
- **阶段 B 完成**（5 切片 + W02 收口：双模式 CLI / 容差比较器自证 / per-producer ECE / 有界场景；F1/F2 兑现）。
- **阶段 C L1 已冻结**：[`docs/design/stage-c-ai-in-loop/概要设计.md`](docs/design/stage-c-ai-in-loop/概要设计.md)——ORT（CPU EP 参考 / CUDA EP 性能）先行、TensorRT EP 预留、适配器内双 provider、`--ep` 运行时选择。
- 代码现状：ctest **12/12** 绿；参考模式零外部依赖；600/42 参考哈希 `574361e62eec92b9` / `459ff85132cc103e`（基线字面量）。
- 旧基线（重建对照）：Gate 1 三判据 1106.7 Hz / 适配器零核心改动 / ECE 分离 0.9617。

## 下一件事 = 领取切片 C1

- **票据**：[`.scratch/stage-c-ai-in-loop/issues/01-ort-dependency-isolation.md`](.scratch/stage-c-ai-in-loop/issues/01-ort-dependency-isolation.md)（无阻塞，可立即开始）
- 流程：新开 session 粘入——

  > 实现 .scratch/stage-c-ai-in-loop/issues/01-ort-dependency-isolation.md——先读 spec（`.scratch/stage-c-ai-in-loop/spec.md`）与 L1（`docs/design/stage-c-ai-in-loop/概要设计.md`）及既有构建配置，产出 L2 详细设计给我批准，批准后 TDD 实现。

- 切片链（阶段 C）：C1 依赖隔离 → C2 Observation 多分辨率 + Camera → C3 RT-DETRv2-S → C4 DA-V2-Small → C5 判据 (c) 重测 + 收口。

## 出口判据（阶段 A / B 均已达成）

参考模式逐字节不变（全量 ctest 绿）+ 性能模式容差自证（噪声 stub：窗内过、超窗红）。

## 不碰

ONNX Runtime 之外的新依赖、TensorRT EP（预留枚举）、训练模型、Vulkan 渲染、付费大算力；sprint 外新想法进 `docs/17-research/research-queue.md`；**无批准的 L2 不写代码**（PLAN 规则 #6）。
