# 03: 延迟计量 + 分位数下沉

**What to build:** loop 逐 tick 逐 stage 记录**原始微秒样本**（保真），并把 p50/p95 的计算**下沉进库**（`percentiles()`，口径 = pre-rebuild runner：排序后线性插值）供程序化判读；TickLog 的 `stage_latency` 填实；延迟预算断言挂点就位（初值 = 对齐运行 p50 × 2，实测后标定，数值不在 L1 拍死）。

**Blocked by:** 01（双模式 CLI + 工件 schema）

**Status:** done — 2026-10-16（预授权执行；证据见 ## Comments）

**流程：** L2 详细设计 → owner 批准 → TDD → problem-log → 走查。

- [x] 逐 stage 原始样本（map<stage, vector<double>>）由 loop 保存；样本含各适配器展开的 `detect:<name>` / 预测键
- [x] `percentiles()` 库内接口（排序后线性插值）——用已知样本集验证插值行为与旧 runner 口径一致
- [x] TickLog/report 呈现逐 stage p50/p95；预算断言挂点就位（`ToleranceWindows` 可承载延迟键）
- [x] 墙钟不进哈希回归：分位数实现后同 seed 双跑 `world_hash`/`ledger_hash` 仍逐位一致
- [x] 参考模式逐字节不变（常驻回归）

## Comments

- 2026-10-16 · **TDD**：slice09（4 组）两轮迭代（先修 `lat_sum` 声明遗漏；再修预算挂点测试的构造错误——自比恒 delta 0，正确语义是“报告键 vs 预算值表”）。全量 ctest **10/10 绿**（ow_slice09 0.03s）。
- 2026-10-16 · **参考模式零行为变化（关键证据）**：`--ticks 600 --seed 42` → `world_hash=574361e62eec92b9` / `ledger_hash=459ff85132cc103e`，与 2026-10-14 对齐基线**逐位一致**（新增插桩不泄漏进哈希）。
- 2026-10-16 · **实测口径**：Debug 下 50 tick 的 JSON stage_percentiles（µs）：adjudicate p50 34.8 / detect:blob_detect 47.2 / render 23.0 / metrics 18.2 / predict:linear 22.4 / **truth_step p50 = 0.1（8 实体单步在 QPC 分辨率边缘，如实记录：预算窗口按重 stage 设定，已在 L2 §5 边界表列明）**。
- 2026-10-16 · **走查**：diff = `src/loop/driver.{hpp,cpp}` + tests/CMake/文书——与票据声明一致；B2 比较器被复用于预算挂点（无新模块）。
- 2026-10-16 · 新坑：过快的 stage 记 0.0μs 是计时事实非缺陷（L2 已列）。
