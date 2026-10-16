# 03: 延迟计量 + 分位数下沉

**What to build:** loop 逐 tick 逐 stage 记录**原始微秒样本**（保真），并把 p50/p95 的计算**下沉进库**（`percentiles()`，口径 = pre-rebuild runner：排序后线性插值）供程序化判读；TickLog 的 `stage_latency` 填实；延迟预算断言挂点就位（初值 = 对齐运行 p50 × 2，实测后标定，数值不在 L1 拍死）。

**Blocked by:** 01（双模式 CLI + 工件 schema）

**Status:** ready-for-agent

**流程：** L2 详细设计 → owner 批准 → TDD → problem-log → 走查。

- [ ] 逐 stage 原始样本（map<stage, vector<double>>）由 loop 保存；样本含各适配器展开的 `detect:<name>` / 预测键
- [ ] `percentiles()` 库内接口（排序后线性插值）——用已知样本集验证插值行为与旧 runner 口径一致
- [ ] TickLog/report 呈现逐 stage p50/p95；预算断言挂点就位（`ToleranceWindows` 可承载延迟键）
- [ ] 墙钟不进哈希回归：分位数实现后同 seed 双跑 `world_hash`/`ledger_hash` 仍逐位一致
- [ ] 参考模式逐字节不变（常驻回归）
