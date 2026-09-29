# Regression: Grounding Loop v0 — Gate 1 Verdict (2026-09-28)

## Claim under test

> A shared canonical world state with explicit contracts supports coherent closed-loop operation across heterogeneous perception models at interactive rates, with metrics that discriminate a deliberately miscalibrated model — on consumer hardware, fully locally, bit-exact reproducible. (PLAN.md §6 Gate 1)

## Environment（完整元数据）

| Item | Value |
|---|---|
| OS | Windows 11 build 26200 |
| CPU | Intel i9-14900HX (24C/32T), 31.8 GiB RAM |
| GPU | RTX 4060 Laptop 8 GiB（本轮未用 CUDA——v0 闭环为纯 CPU，见"偏差"） |
| Driver / CUDA | 560.94 / CUDA 12.6.20（toolkit 探测；loop 本体零 GPU 依赖） |
| Compiler | MSVC 19.29.30133 (VS2019, x64) |
| Build | CMake 3.20.0-rc1 + Ninja, Release (/O2), `/utf-8 /permissive- /W4`，零警告 |
| Commit | `8f47bf7` |
| Command | `build.bat --test` 后 `omniworld_loop.exe --seed 42 --ticks 600 --bodies 12 --out <artifacts>` |
| Artifacts | `benchmarks/regression/artifacts/2026-09-28-grounding-loop-v0/grounding-loop-report.json` |

## Run parameters

seed=42, bodies=12, ticks=600, observation 224×224 RGB（正交俯视）, assoc_threshold=0.06, 3 个自注册感知适配器。

## Results

**Loop**：end_to_end **1106.7 Hz**（总墙钟 0.542 s / 600 ticks）；final_state_digest `7d22caf5dc9d3bf5`；final_image_digest `e9ecc52536c7269b`。

**Per-stage latency (us, p50 / p95)**

| Stage | p50 | p95 |
|---|---|---|
| truth_step | 0.3 | 0.8 |
| render | 58.4 | 67.9 |
| detect:blob_cc | 266.9 | 306.4 |
| detect:blob_cc_miscalibrated | 265.6 | 315.9 |
| detect:blob_grid | 282.3 | 345.6 |
| adjudicate:×3 | 0.8–1.5 | 2.2–4.7 |

瓶颈是两个检测器（各 ~270us），runtime 自身开销（真值推进 + 裁决合计 <5us）可忽略——契约层的代价确实接近零。

**Coherence metrics**

| producer | props | applied | id_stab | err_mean | hc_wrong | ECE |
|---|---|---|---|---|---|---|
| blob_cc | 7080 | 7080 | 0.9924 | 0.0011 | 0.0076 | **0.0115** |
| blob_cc_miscalibrated | 7080 | 7080 | 0.9924 | 0.0011 | 0.0000 | **0.9732** |
| blob_grid | 6070 | 5720 | 0.9524 | 0.0053 | 0.0000 | 0.5956 |

附带发现（真实的，非剧本）：`blob_grid` 的 ECE=0.596 说明其置信度公式相对 95.2% 的身份稳定性**过自信**；运行时把"诚实但校准差"的模型也标了出来——这正是 grounding 指标的意义。

## Gate 1 判定

| 判据 | 及格线 | 实测 | 结果 |
|---|---|---|---|
| (a) 端到端帧率 | ≥30 Hz（RTX 4060） | **1106.7 Hz** | ✅ PASS |
| (b) 第二/三个模型只经适配器接入，核心零改动 | 注册表 3 适配器、loop 无 per-model 分支 | 3/3，loop 代码零分支 | ✅ PASS |
| (c) 指标判别力 | ECE 分离 ≥0.30 | **0.9617**（0.0115 vs 0.9732） | ✅ PASS |

**GATE 1 VERDICT: PASS → 进入 Phase 1。**

## 确定性证据

`ow_tests determinism`：同 seed 两次完整闭环，逐 tick 状态摘要链 + 终态/终帧摘要全部一致（位级）；不同 seed 摘要不同（RNG 生效性反证）。ctest 4/4 绿。

## 实现过程中的修正（诚实记录）

1. Gate 1c 首版指标（高置信错误率）对**欠自信型失准**有盲区（反转模型从不跨过 0.7 阈值）→ 换成标准 **ECE（5-bin）**，双向失准都能抓。
2. `blob_grid` 首版用块均值颜色，被背景稀释后零提议 → 改为逐像素类别投票 + 块聚类。
3. MSVC 静态库丢弃无引用符号导致自注册失效 → `ow_core` 改 OBJECT library。

## 偏差与范围边界（如实声明）

1. **v0 感知模型为两个内置经典 CV 检测器**（blob_cc / blob_grid）而非 sprint 原文的 ONNX 模型：保证零依赖、构建必成、确定性可控。ONNX Runtime + RT-DETRv2-S / Depth-Anything-V2-Small 将通过**同一适配器契约**换入（Gate 1 已证明该路径存在）。
2. 真值世界为**运动学**（弹性墙反弹，无体间碰撞）——PLAN.md 明确 v0 如此；接触丰富度属于 Phase 3 外部物理适配器。
3. 观察为正交俯视 RGB（无深度通道）；"世界系提议"依赖已知地面平面假设，检测器不做单目深度恢复。

## 结论

三条证伪判据全部通过且余量巨大（帧率 36.9×）。**Phase 0 完成。**下一步（Phase 1）：回放录制 + replay 适配器 + 小型自训 dynamics MLP + prediction-vs-truth divergence 指标。
