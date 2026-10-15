# 06: 世界模型接缝 + Gate-1 对齐报告

**What to build:** WorldModelAdapter 第二接缝 + 最小世界模型集（replay / linear / zero 各为单文件适配器）+ 完整 tick 主循环编排（TickLog：applied/rejected + 逐阶段延迟）→ 产出阶段 A 出口工件：Gate-1 对齐报告。

**Blocked by:** 05（指标组）

**Status:** ready-for-agent

**流程：** L2 详细设计 → owner 批准 → TDD 实现 → problem-log → 走查。

- [ ] 世界模型经第二接缝单文件接入（replay=对照 oracle / linear / zero_delta 最小集），契约家族无关性实证
- [ ] loop 全管线端到端跑通（编排顺序 = L0 §3.2 时序图）；每 tick 产生 TickLog（applied/rejected + 逐阶段延迟，契约⑨）
- [ ] 适配器异常隔离：抛异常的 stub 适配器不阻断 tick 推进（自切片 03 移交的验收条件）
- [ ] **Gate-1 对齐报告**落 `benchmarks/regression/`：新实现三判据（闭环速率 / 适配器零核心改动 / ECE 判别）vs `pre-rebuild` 基线（1106.7 Hz / 0.9617）并排，环境元数据 + 可复现命令，差异逐条如实解释
- [ ] 全量 ctest 绿；完整闭环同 seed 逐字节复现
- [ ] 对照 CURRENT/PLAN 的阶段 A 出口判据逐条自查，未达标项如实标注
