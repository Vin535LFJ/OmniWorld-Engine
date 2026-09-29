# CURRENT — 本周唯一焦点

> 只回答一个问题：**现在做什么。** 背景与全貌见 [`PLAN.md`](PLAN.md)。

**Phase 0 + Phase 1 已于 2026-09-28 完成（Gate 1 三判据 PASS；五世界模型 divergence 分离实证）。**

## 当前状态

- 代码：`src/`（world_state / truth_world / observation / perception / grounding_loop / models）+ `apps/omniworld_loop` + `tests/`，ctest 5/5 绿，零外部依赖，位级确定性。
- 报告：`benchmarks/regression/2026-09-28-grounding-loop-v0.md`（Gate 1）+ `2026-09-28-phase1-world-models.md`（Phase 1）。
- 复现：`build.bat --test`；`build\omniworld_loop.exe`（Gate 1）/ `--mode phase1`（divergence）。

## 下一件事（按 PLAN.md §6 Phase 2，择一开工）

1. **ONNX 换装**：用同一感知适配器契约把 RT-DETRv2-S / Depth-Anything-V2-Small 换入（替换内置 CV 检测器）——第一次"真模型"接入，兼作适配器不变式的再验证。
2. **动作词汇对齐**：研究 WorldMark 的 W S A D 词汇表并设计我们的 Action 契约映射（研究笔记，不动代码）。
3. **公开 v0.1 打磨**：README 对外版 + 一页工程笔记（Gate 1 + Phase 1 数字）作为敲门砖工件。

## 不碰

训练任何模型、ROS 2/VLA/LLM、分布式、游戏 demo、编辑器、商业化；sprint 外新想法进 `docs/17-research/research-queue.md`。
