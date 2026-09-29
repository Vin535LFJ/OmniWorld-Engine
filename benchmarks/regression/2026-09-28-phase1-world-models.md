# Regression: Phase 1 — World Models in the Loop (2026-09-28)

## Claim under test

> 世界模型（含学习型）以适配器进入同一闭环后，prediction-vs-truth divergence 指标能将 oracle / 经典基线 / 学习模型 / 地板干净分离；且新模型接入 = 单个自注册文件，核心零改动。（PLAN.md §6 Phase 1 出口）

## Environment

同 [`2026-09-28-grounding-loop-v0.md`](2026-09-28-grounding-loop-v0.md)（Windows 11 26200 / i9-14900HX / MSVC 19.29.30133 / CMake+Ninja Release / 零外部依赖）。Commits: `030ec5f`（四模型）→ `c902ad5`（第五模型单文件接入）。ctest **5/5 绿**（含 rollout 位级一致、MLP 训练确定性、oracle=0、基线排序）。

## Run parameters

seed=42, bodies=12, ticks=600（7200 条 entity-tick 预测/模型）；rollout_digest `4c594db6962cd149`；模型预测在真值步进**之前**做出（无前瞻）；divergence = |预测位置 − 实际下一位置|（平面欧氏）。

## Results（4 模型 → +damped_linear 5 模型）

| model | mean_div | max_div | fit_s | pred p50 (us) | 定位 |
|---|---|---|---|---|---|
| replay（oracle） | **0.000000** | 0.000000 | 0 | 0.8 | 管线正确性证明 |
| linear（外推基线） | 0.000218 | 0.034194 | 0 | 0.2 | 自由段精确、墙反弹错 |
| damped_linear（半速外推） | 0.006907 | 0.025465 | 0 | 0.2 | 单文件新接入即入榜 |
| mlp_dynamics（自训 MLP） | 0.011737 | 0.029176 | 0.086 | 1.8 | 学到"delta≈vel"，未学到反弹 |
| zero_delta（静止地板） | 0.013674 | 0.018746 | 0 | 0.3 | 最差基线 |

**分离完备**：五个模型两两可分，且序符合理论预期（0 < 自由段误差 < 半速 < 学习模型 < 静止）。MLP max_div 低于 linear（0.029 vs 0.034）——学习模型在墙附近表现更保守，指标把这种质性差异也显式化了。

## Phase 1 出口判定

| 判据 | 结果 |
|---|---|
| 回放数据 + 小 dynamics 模型进闭环 | ✅ replay + 纯 C++ 确定性训练的 ~3k 参数 MLP 均经同一适配器契约进入 |
| prediction-vs-truth divergence 指标 | ✅ 上表，五模型分离 |
| 新模型只写 1 个适配器文件、核心零改动 | ✅ `c902ad5` 仅新增 `damped_linear.cpp`（+1 行 CMake 源列表），loop/契约/测试零改动，评测自动发现新模型 |
| 首份对外 benchmark 笔记 | ✅ 本文件 + Gate 1 报告，全部数字可由 `build.bat --test` + 两条命令复现 |

**PHASE 1 EXIT: PASS。**

## 诚实记录（实现期修正与事故）

1. MLP 首版预测**绝对位置**，30 epoch 后输出趋近全局均值（div > 静止地板）→ 改为预测 **delta**（近线性映射，即收敛且余量可学墙反弹）。
2. `omniworld_loop.exe` 两次遭 mt.exe 清单写入失败——Windows Defender 对新 exe 的瞬时文件锁；等待后自愈，ctest 复验 5/5。非代码问题。
3. 世界模型在评测中的 confidence 固定为 0.5（逐模型置信度估计属 v0.1 后工作，如实声明）。

## 与 Phase 0 的关系

同一 World Contract v0（实体/提议/tick/所有权）未改动即承载了第二类参与者（世界模型）；感知闭环（Gate 1）与预测闭环（本报告）共享全部状态机制——这正是"一个 canonical 状态服务多类模型"论点的第一次实证。

## 结论

Phase 0 + Phase 1 完成。下一步（Phase 2，按 PLAN.md §6）：3+ 第三方可评测对象、对齐 WorldMark/WBench 动作词汇、公开发布 v0.1 + 首个 OSS PR。
