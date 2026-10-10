# CURRENT — 本周唯一焦点

> 只回答一个问题：**现在做什么。** 背景与全貌见 [`PLAN.md`](PLAN.md)。

**2026-10-10 方向重开定案（ADR-0008/0009）；下一步 = Phase 2「AI 入环」开工。**

## 当前状态

- 已完成：Phase 0 + Phase 1（2026-09-28）——Gate 1 三判据 PASS（1106.7 Hz / 适配器零核心改动 / ECE 分离 0.9617）；五世界模型 divergence 分离；契约 v0.1 冻结。
- 代码：`src/`（world_state / truth_world / observation / perception / grounding_loop / models）+ `apps/omniworld_loop` + `tests/`，ctest 5/5 绿，零外部依赖，参考模式位级确定性。
- 报告：`benchmarks/regression/2026-09-28-grounding-loop-v0.md`（Gate 1）+ `2026-09-28-phase1-world-models.md`（Phase 1）。
- 复现：`build.bat --test`；`build\omniworld_loop.exe`（Gate 1）/ `--mode phase1`（divergence）。

## 下一件事 = Phase 2 · AI 入环（ONNX 换装）

1. **ONNX Runtime（CUDA EP）真模型适配器**：RT-DETRv2-S / Depth-Anything-V2-Small 经同一感知适配器契约接入（替换内置 CV 检测器）——第一次"真 AI 在环"，兼作适配器不变式再验证。
2. **双模式回归框架（ADR-0009）**：参考模式逐字节 oracle 不动；性能模式容差判据 + 延迟预算报告。
3. **折入架构候选 #3**：Camera 并入 Observation、修 224 硬编码（真模型分辨率非 224，必然踩中）。

## 出口条件（Phase 2 能力 Gate）

- 真模型以单文件适配器接入，核心零改动；
- 参考模式 ctest 全绿、同 seed 位级复现保持；
- 性能模式延迟预算实测报告（4060 8GB 环境元数据 + 模式标注），真模型 vs 内置检测器的指标对比入 regression 报告。

## 不碰

付费大算力训练（个人预算红线）；ROS 2/VLA/LLM、分布式、游戏 demo、编辑器、商业化；渲染升级不在关键路径（Vulkan 离屏渲染器 = 可选技能项目）；sprint 外新想法进 `docs/17-research/research-queue.md`。
