# 02: 容差比较器 + 噪声 stub 自证

**What to build:** 容差比较器（`src/tolerance/`，loop 私有子件）——两组指标 map + 每指标容差窗口进，判定（pass/fail + 逐指标差值）出，纯数据进纯数据出；以及一个参数化噪声幅度的 **测试替身** stub 适配器，双向自证：噪声在窗口内 → 过；超窗 → 判红。这就是阶段 B 出口判据「性能模式容差自证（噪声 stub）」的机制。

**Blocked by:** 01（双模式 CLI + 工件 schema）

**Status:** ready-for-agent

**流程：** L2 详细设计 → owner 批准 → TDD → problem-log → 走查。

- [ ] `compare_within_tolerance(a, b, windows) -> ToleranceVerdict`：逐指标 |差| 与窗口比较，输出全部差值（不只第一个失败者）
- [ ] 窗口声明支持聚合键与分解键（如 `"ece:<producer>"`，为 B4 预留）
- [ ] 噪声 stub 为测试替身（不进 `src/`；先例 slice04 `AlwaysReject` / slice06 `ThrowingPerception`），幅度参数化
- [ ] 自证两向实跑：幅度 < 窗 → pass；幅度 > 窗 → fail（测试输出注明方向）
- [ ] 不依赖真模型与 ONNX Runtime（零外部依赖保持）
