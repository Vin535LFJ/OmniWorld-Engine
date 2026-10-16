# 04: per-producer ECE（F1）

**What to build:** Metrics 深模块内实现 **per-producer ECE**——按 producer 分桶，report 同时给出聚合键 `"ece"` 与分解键 `"ece:<producer>"`（平铺、排序确定）；然后**重跑对齐场景**并如实分析判据 (c) 的在环可判性（诚实 vs 夸大检测器能否分离），兑现 problem-log 005 的教训（聚合必须附按来源分解，反模式 #13 的正面案例）。

**Blocked by:** 02（容差比较器——分离判定可编程）、03（延迟/分位数与工件面）

**Status:** done — 2026-10-16（预授权执行；证据见 ## Comments）

**流程：** L2 详细设计 → owner 批准 → TDD → problem-log → 走查。

- [x] ECE 指标按 producer 名分桶；判定口径沿用 Round 1 #2（good = 正确 id 且误差 ≤ 0.05）
- [x] report 聚合 + 分解同层：`"ece"` 与 `"ece:<producer>"` 并存，`std::map` 排序确定
- [x] 单测：两个 producer 不同校准度的**精确分离**（沿用 slice05 的全二进制精确法）
- [x] 对齐场景重跑（沿用 2026-10-14 报告参数）：产出 per-producer ECE 表；**如实分析**在环可判性（能分离 → 给出分离量；不能 → 记录主导因素）
- [x] 参考模式逐字节不变；聚合与分解数值一致性断言（分解加权复原聚合）

## Comments

- 2026-10-16 · **TDD**：slice10（2 组）两轮迭代（首轮修正断言：聚合区间需剔除模型生产者——per-producer 语义本就含模型，测试场景改为感知专用例）。全量 ctest **11/11 绿**（ow_slice10 0.02s）。
- 2026-10-16 · **对齐场景重测（600 tick / seed 42 默认场景，per-producer ECE）**：`ece:blob_detect = 0.198904`（诚实、欠自信）；`ece:blob_detect_overconfident = 0.010000`；**分离量 = 0.188904 < 旧阈值 0.30**；模型侧 `ece:linear_extrapolation` / `ece:zero_delta` 均 0.500000（自报 0.5 对准确率≈1）。**如实裁定：判据 (c) 在环 PARTIAL 持续**——分解机制与可编程判定已就位（F1 兑现），分离幅度低于 pre-rebuild 阈值；原因与重测计划见 problem-log 006。
- 2026-10-16 · **走查**：diff = `src/metrics/metrics.{hpp,cpp}`（Metric::collect 默认实现 + producer 分桶）+ tests/CMake/文书——键约定与 L1 §4 一致，B2 比较器零改动直接可消费分解键。
- 2026-10-16 · 已知限制（如实）：分离幅度依赖场景参数；F2 有界场景（墙反弹）落地后需重测。
