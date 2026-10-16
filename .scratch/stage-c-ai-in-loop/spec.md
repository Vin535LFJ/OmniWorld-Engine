# Spec — 阶段 C「AI 入环」

Status: ready-for-agent

> 来源：L1 已冻结（docs/design/stage-c-ai-in-loop/概要设计.md，Round 1+2 六项裁决，2026-10-16）+ PLAN §六 Phase 2 行 + 09-24 复用地图 + L0 §2 契约⑧ + ADR-0009。工具无关：任何 AI 按本 spec + L1 + L0 可领任务。

## Problem Statement

引擎目前是"参考模式单声道"：感知参与者只有两个内置 CV 检测器（CPU、零外部依赖）。这验证了契约，但没有验证引擎的立身之本——**真实 AI 推理模型以 GPU 速度穿过同一条契约**。阶段 C 引入第一个外部依赖（ONNX Runtime）与第一个 GPU 路径，同时必须保持参考模式的逐字节 oracle 属性与零依赖构建。

## Solution

按 L1 §5 五片推进：C1 依赖隔离（CMake 选项 + research note + fetch 脚本）→ C2 Observation 多分辨率 + Camera 合并（候选 #3）→ C3 RT-DETRv2-S 双 provider 适配器 → C4 DA-V2-Small 适配器 → C5 判据 (c) 重测 + 阶段报告。真模型经同一感知接缝单文件接入；provider 运行时选择（--ep cpu 默认参考 / --ep cuda 性能）。

## User Stories

1. As the engine owner, I want real pretrained models (RT-DETRv2-S, Depth-Anything-V2-Small) running in the loop, so that the engine demonstrably serves real AI inference, not built-in detectors.
2. As the engine owner, I want the reference build to stay zero-external-dependency, so that clone-and-verify still works everywhere without ONNX Runtime.
3. As the engine owner, I want ONNX Runtime as an optional dependency linked only into the performance path, so that the oracle property survives to the model side.
4. As an adapter author, I want a single-file adapter holding both CPU EP and CUDA EP providers, so that one adapter serves both modes with no core branching.
5. As the engine owner, I want EP selection at runtime (`--ep cpu|cuda`), so that mode and provider compose without rebuilds.
6. As the engine owner, I want a fetch script with SHA256 verification and license records for the model files, so that large binaries stay out of git but remain verifiable.
7. As an adapter author, I want observations at model-required resolutions (640×640 / ~518×518) with the camera as part of the observation, so that input shapes are properties, not hardcodes (candidate #3).
8. As the engine owner, I want per-adapter tolerance windows declared for performance mode (ADR-0009:16), so that GPU nondeterminism is bounded and documented.
9. As a portfolio viewer, I want a latency-budget regression report for the real models on the 4060, so that GPU claims are evidence-backed.
10. As the engine owner, I want criterion (c) re-measured in the real-model scene, so that the PARTIAL verdict from stages A/B gets its honest closure attempt (problem-log 006/007).

## Implementation Decisions

- 框架：ONNX Runtime（CPU EP + CUDA EP）；TensorRT EP 预留枚举不实现（L1 Round 1 #2；C1 research note 核验 Windows 分发近况后另议）。
- Provider 运行时选择（RunConfig.ep / CLI --ep，默认 cpu）。
- 适配器内双 provider（同一模型文件）；ORT 仅链入性能路径，核心模块零 include。
- Observation：Camera 并入、分辨率属性化（C2）。
- 文件夹：模型文件不入 git；tools/fetch_models + SHA256 于 research note。
- 容差政策：窗口 = 实测最大抖动 × 2，切片内标定（Round 1 #4）。

## Testing Decisions

- 行为 + 确定性断言成对（problem-log 002）；参考模式基线哈希字面量断言（600/42 默认场景）。
- 默认构建（ORT OFF）零依赖不变式的构建级证明；ORT ON 时的编译隔离。
- 真模型：参考路径（CPU EP）逐字节；性能路径（CUDA EP）容差窗口 + 延迟预算。
- 适配器不变式：单文件 + git diff 证据（damped_linear 先例同款）。

## Out of Scope

TensorRT EP 实现（预留枚举，核验后另排）；模型训练（阶段 4 / 族谱③）；Vulkan 渲染；NVDEC 视频输入（阶段 4）；ROS 2 / 分布式；评测工具产品化（期权分支）。

## Further Notes

- C1 research note 是一手来源核验：ORT 版本与 Windows CUDA EP、TensorRT Windows 分发近况、两模型的 ONNX 导出路径与许可、文件体积。
- 实现中问题随遇随记 docs/design/problem-log.md；STAR 素材阶段末进 docs/interview-narrative.md。
