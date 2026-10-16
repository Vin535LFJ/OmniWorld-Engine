# 04: per-producer ECE（F1）

**What to build:** Metrics 深模块内实现 **per-producer ECE**——按 producer 分桶，report 同时给出聚合键 `"ece"` 与分解键 `"ece:<producer>"`（平铺、排序确定）；然后**重跑对齐场景**并如实分析判据 (c) 的在环可判性（诚实 vs 夸大检测器能否分离），兑现 problem-log 005 的教训（聚合必须附按来源分解，反模式 #13 的正面案例）。

**Blocked by:** 02（容差比较器——分离判定可编程）、03（延迟/分位数与工件面）

**Status:** ready-for-agent

**流程：** L2 详细设计 → owner 批准 → TDD → problem-log → 走查。

- [ ] ECE 指标按 producer 名分桶；判定口径沿用 Round 1 #2（good = 正确 id 且误差 ≤ 0.05）
- [ ] report 聚合 + 分解同层：`"ece"` 与 `"ece:<producer>"` 并存，`std::map` 排序确定
- [ ] 单测：两个 producer 不同校准度的**精确分离**（沿用 slice05 的全二进制精确法）
- [ ] 对齐场景重跑（沿用 2026-10-14 报告参数）：产出 per-producer ECE 表；**如实分析**在环可判性（能分离 → 给出分离量；不能 → 记录主导因素）
- [ ] 参考模式逐字节不变；聚合与分解数值一致性断言（分解加权复原聚合）
