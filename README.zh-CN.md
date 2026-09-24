# OmniWorld Engine

[English README](README.md)

📖 **跟丢了项目现状？从 Owner 导读系列开始读 → [`docs/19-owner-briefing/`](docs/19-owner-briefing/)**

**OmniWorld Engine 是一个开放的 runtime：让感知模型、世界模型、仿真器、渲染器和智能体，在同一个可测量的、持续演化的世界状态上保持一致。**

交互式世界模型正被要求成为"世界模拟器"和"训练智能体的环境"，但今天没有任何开放系统让它们与真值仿真器、智能体共享一个带明确契约（身份 identity / 时间 time / 所有权 ownership / 置信度 confidence）的活世界状态——这个能力要么被锁在 NVIDIA 的专有栈里（Mega Omniverse Blueprint / `ovstage`），要么在每个交互世界模型评测项目里被手工重写。OmniWorld 就是这个闭环的开放中立版本：grounding 被**测量**，而不是被假设。

## 🚧 当前开发焦点

Phase：**Phase 0 — Grounding Loop v0**（前 30 天；方向决策 2026-09-21，见 [`ADR-0007`](docs/01-architecture/decisions/ADR-0007-project-direction.md)）

当前目标：在 RTX 4060 8 GB 上证明最小可行命题——一个确定性合成世界、两个本地感知适配器、所有权裁决、连贯性指标，闭环以交互级帧率运行，并满足三条可证伪判据（Gate 1，见 [`current-sprint.md`](docs/02-roadmap/current-sprint.md)）。

状态：**规划阶段已于 2026-09-21 关闭并冻结**。`runtime_probe`（C++20 CPU/CUDA/Vulkan 硬件探针）可构建且已提交；第一份 regression baseline 见 `benchmarks/regression/2026-09-18-runtime-probe-baseline.md`。

## 为什么是 now

- 2025–2026 交互式世界模型评测生态爆发且碎片化（WBench、WorldMark、WorldArena、KineBench…）：每个团队都在重复实现 action 适配器、状态 schema、仿真耦合。WorldMark 原话：*"no shared protocol has been established"*。
- 世界运行时的"工业版"是 NVIDIA 专有、生态内闭环；AOUSD 已定稿规范只覆盖场景描述，不含运行时语义。
- 没有任何开放玩家在运 runtime 层：Genie 3 无 API；World Labs 交付世界生成与持久资产，不是活状态的 runtime。

完整证据：[`docs/17-research/2026-09-21-direction-decision.md`](docs/17-research/2026-09-21-direction-decision.md)。

## 它是什么 / 不是什么

- **Core**：World Contract v0（`docs/01-architecture/world-state.md`）、闭环 runtime（版本化 canonical 状态、帧预算计量）、grounding 指标（身份稳定性、transform 误差 vs 真值、时间连贯性）、确定性真值世界。
- **Adapter**：本地 ONNX 感知模型、回放数据、小型 dynamics 模型（Phase 1）、外部物理引擎（Phase 3），以及可选的 World API 场景适配器（绝不进 benchmark 主线）。
- **不是**：工业数字孪生平台、世界模型训练项目、游戏引擎/编辑器/渲染器替代、"连接一切"的平台。规则：说不出"只有 OmniWorld 能测的指标"的功能不进 Core。

## 开发入口

- 每日入口：[`CURRENT.md`](CURRENT.md)
- 当前 Sprint（30 天）：[`docs/02-roadmap/current-sprint.md`](docs/02-roadmap/current-sprint.md)
- 唯一主线路线图 + 决策门：[`docs/02-roadmap/master-roadmap.md`](docs/02-roadmap/master-roadmap.md)
- World Contract v0：[`docs/01-architecture/world-state.md`](docs/01-architecture/world-state.md)
- 决策记录：[`docs/01-architecture/decisions/`](docs/01-architecture/decisions/)
- 研究队列：[`docs/17-research/research-queue.md`](docs/17-research/research-queue.md)
- Benchmarks：[`benchmarks/`](benchmarks/)

## 构建与运行

```bash
cmake -B build -G Ninja && cmake --build build
./build/runtime_probe            # 硬件探针（CPU/CUDA/Vulkan 能力）
./build/runtime_probe --json     # 机器可读输出
ctest --test-dir build           # 冒烟测试
```

要求：C++20 编译器、CMake + Ninja；CUDA Toolkit 与系统 Vulkan loader 自动检测（可选）。Grounding Loop 按 Phase 0 推进（见 sprint）。

## 语言规则

- 项目架构/治理文档：英文优先。
- 研究报告与知识库：中文优先，技术名词保留英文原文。
