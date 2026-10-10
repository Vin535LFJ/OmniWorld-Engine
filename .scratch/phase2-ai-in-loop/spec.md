# Spec — Phase 2「AI 入环」（ONNX 换装 + 双模式回归框架）

Status: ready-for-agent

> 来源：2026-10-10 方向重开访谈（ADR-0008/0009）+ CURRENT.md 出口条件。词汇遵循 GLOSSARY.md。

## Problem Statement

引擎的 Grounding Loop 目前用内置 CV 检测器（blob_cc / blob_grid）充当 Perception Adapter——这是 v0 时代声明的临时偏离。ADR-0008 之后项目身份是 GPU 原生世界引擎，但代码库还没有一行 GPU 代码、没有真 AI 在环；owner 的目标函数（磨练技术 ≳ AI 岗敲门砖）要求真模型在环 + 实测延迟预算。同时铁律"同 seed 逐字节复现"与 GPU 推理天然冲突，ADR-0009 已在契约层裁决为双模式，但 runtime 还没有实现它的框架。另外 Observation/Camera 存在 224 硬编码潜伏 bug（架构评审候选 #3），真模型的输入分辨率不是 224，接入时必然踩中。

## Solution

用 ONNX Runtime（CUDA EP）把两个真实预训练模型（RT-DETRv2-S 检测、Depth-Anything-V2-Small 深度）经**既有的单文件 Perception Adapter 契约**接入闭环，替换内置 CV 检测器——第一次"真 AI 在环"，同时再验证适配器不变式。加双模式回归框架：参考模式（纯 CPU）保持逐字节复现、仍是契约回归唯一 oracle；性能模式（GPU）以容差窗口 + 延迟预算为判据。折入架构候选 #3（Camera 并入 Observation、修 224 硬编码），使任意分辨率成为 Observation 的属性。

## User Stories

1. As the engine owner, I want a real pretrained perception model running inside the Grounding Loop, so that the engine demonstrates real AI in the loop instead of CV placeholders.
2. As the engine owner, I want RT-DETRv2-S attached through the same single-file Perception Adapter contract as the built-in detectors, so that the adapter invariant (new model = one file, zero core changes) is revalidated by a real model.
3. As the engine owner, I want Depth-Anything-V2-Small attached as a second real model family, so that the contract proves it is family-agnostic (detection + depth).
4. As the engine owner, I want the loop to run in reference mode (CPU) with byte-exact reproducibility unchanged, so that all existing contract regression tests remain the oracle.
5. As the engine owner, I want a performance mode that runs ONNX Runtime on the CUDA EP, so that GPU latency budgets are measurable on RTX 4060 8GB.
6. As the engine owner, I want performance-mode regression judged by tolerance windows + latency budget instead of byte equality, so that GPU nondeterminism never causes false test failures (ADR-0009).
7. As the engine owner, I want a per-stage latency breakdown in performance mode, so that the benchmark note shows where milliseconds go (preprocess → inference → postprocess → adjudication).
8. As an adapter author, I want the adapter to receive Observation frames at the model's required resolution rather than a hardcoded 224, so that models with different input shapes work unchanged.
9. As an adapter author, I want preprocessing (resize / normalize) to be adapter-owned, so that each model controls its own input format within the contract.
10. As a developer cloning the repo, I want reference mode to remain zero-external-dependency, so that `build.bat --test` → ctest green still works without ONNX Runtime installed.
11. As a developer cloning the repo, I want performance mode to fail with a clear message when ONNX Runtime / model file / CUDA is absent, so that missing optional dependencies degrade gracefully and never affect reference mode.
12. As a developer cloning the repo, I want a documented way to fetch the two ONNX model files with license provenance, so that reproducing performance mode is unambiguous.
13. As a portfolio viewer (recruiter), I want a regression report with environment metadata + mode labels comparing built-in detectors vs real models on identity stability / Divergence / latency, so that the engine's claims are independently verifiable.
14. As the engine owner, I want the 224 hardcode eliminated by merging Camera into Observation (arch candidate #3), so that resolution is a property of the observation, not a magic constant.
15. As the engine owner, I want real-model confidence to flow through the same Proposal confidence field, so that ECE calibration metrics apply to real models unmodified.
16. As the engine owner, I want tolerance windows declared per model adapter, so that each GPU component states its own reproducibility contract (ADR-0009 consequence).

## Implementation Decisions

- 推理框架 = ONNX Runtime（MIT）；TensorRT 留作后续选项（族谱②"ONNX Runtime 先行顶位"）。许可红线遵循 PLAN §五：RT-DETRv2-S（Apache-2.0）、Depth-Anything-V2-Small（Apache-2.0，仅 Small）。
- 模型经既有自注册单文件 Perception Adapter 契约（OW_REGISTER_*）接入；loop 内保持零逐模型代码。
- ONNX Runtime 是**可选依赖**：参考模式（默认）在无它时照常构建与运行；性能模式需要它。ONNX 适配器文件仅在依赖存在时编译（build flag），不触碰核心文件。
- 双模式框架：参考模式 = 现有逐字节路径，契约回归唯一 oracle 不动；性能模式 = 新 runner 路径 + CLI 模式开关，判据为固定 seed 容差 + 延迟预算。
- 预处理归适配器所有（按模型契约 resize/normalize）；Observation 提供帧 + 元数据；Camera 并入 Observation（候选 #3），224 常量成为 Observation 的属性。
- 真模型置信度映射到 Proposal.confidence，语义不变；每个 GPU 适配器声明自己的容差窗口。
- 延迟计量沿用 WorldTransition 的 per_stage_latency 字段；帧元数据按内存契约标注 backend/device（性能模式如实标注 GPU）。
- World Contract v0.1 字段语义零改动（ADR-0009：只分层运行模式层的可复现性承诺）。

## Testing Decisions

- 只测 external behavior：适配器不变式（新模型注册即跑、核心零 diff——damped_linear 有先例）、参考模式逐字节回归（现有 ctest 套件保持全绿且不变）、性能模式容差回归（固定 seed、容差窗口、延迟预算达标）。
- 好测试断言契约可见行为（Proposal、裁决结果、指标、延迟分项），绝不断言内部实现。
- 被测模块：perception adapter registry、两种模式下的 grounding loop、指标（真模型 vs 内置对比）、Observation（分辨率属性）。
- Prior art：Gate-1 regression 报告格式（环境元数据 + 可复现命令）与 Phase-1 divergence 套件；适配器不变式有 git 证据（damped_linear 单文件提交）。

## Out of Scope

- Vulkan 离屏渲染器（可选技能项目，不在关键路径）。
- NVDEC / 真实视频回放输入（Phase 3）。
- Kaggle 小模型训练（Phase 4）。
- 架构候选 #1 / #2 / #4 / #5（挂起，按需领取）。
- WorldMark 动作词汇对齐、评测工具产品化（期权分支）。
- TensorRT 后端、量化、INT8。
- 逐模型置信度估计（仍属 v0.2 工作）。

## Further Notes

- 架构评审报告（`docs/17-research/2026-10-09-architecture-review.html`）含候选 #3 的完整理由与 224 潜伏 bug 位置。
- Windows + ONNX Runtime CUDA EP 的可行性核验属于第一个 ticket（research note，遵循 PLAN 规则 #3：研究只解当前 phase 出口所需的问题）。
- loop driver 合并（候选 #5）挂起——性能模式应落在现有 Gate-1/phase1 driver 形态上，不做预重构。
