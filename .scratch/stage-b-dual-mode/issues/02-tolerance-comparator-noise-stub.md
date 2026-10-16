# 02: 容差比较器 + 噪声 stub 自证

**What to build:** 容差比较器（`src/tolerance/`，loop 私有子件）——两组指标 map + 每指标容差窗口进，判定（pass/fail + 逐指标差值）出，纯数据进纯数据出；以及一个参数化噪声幅度的 **测试替身** stub 适配器，双向自证：噪声在窗口内 → 过；超窗 → 判红。这就是阶段 B 出口判据「性能模式容差自证（噪声 stub）」的机制。

**Blocked by:** 01（双模式 CLI + 工件 schema）

**Status:** done — 2026-10-16（预授权执行；证据见 ## Comments）

**流程：** L2 详细设计 → owner 批准 → TDD → problem-log → 走查。

- [x] `compare_within_tolerance(a, b, windows) -> ToleranceVerdict`：逐指标 |差| 与窗口比较，输出全部差值（不只第一个失败者）
- [x] 窗口声明支持聚合键与分解键（如 `"ece:<producer>"`，为 B4 预留）
- [x] 噪声 stub 为测试替身（不进 `src/`；先例 slice04 `AlwaysReject` / slice06 `ThrowingPerception`），幅度参数化
- [x] 自证两向实跑：幅度 < 窗 → pass；幅度 > 窗 → fail（测试输出注明方向）
- [x] 不依赖真模型与 ONNX Runtime（零外部依赖保持）

## Comments

- 2026-10-16 · **TDD**：slice08（4 组）一次编写即绿（修两处编译级疏漏：缺 `<cmath>`/头 include）；全量 ctest **9/9 绿**（ow_slice08 0.04s）。
- 2026-10-16 · **出口判据机制达成**：噪声 stub（测试替身、参数化 ε）双向自证——小噪声（ε=0.02）在 declared window（divergence±0.05）内 **pass**；大噪声（ε=0.2）超窗 **fail** 且 deltas 输出全部被比较键的精确差值（不只布尔）。AD：阶段 B 退出判据“性能模式容差自证（噪声 stub）”的机制已就位。
- 2026-10-16 · **走查**：diff = `src/tolerance/`（新增，纯函数、无 I/O/时钟/RNG）+ tests/CMake/文书——比较器无新参与者进 src；stub 留在 tests（与 L2 §3 裁定一致）。
- 2026-10-16 · 新坑：无（口径裁定见 L2 §2/§5：缺键哨兵 -1.0、空窗 pass、恰等判过）。
