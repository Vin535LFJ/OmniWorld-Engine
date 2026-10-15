# 05: 指标组

**What to build:** Metrics 深模块——一个 feed 入口（真值 + 信念账本 + Tick 日志），divergence / ECE / 身份稳定性在内部可插拔注册；加新指标不动核心。

**Blocked by:** 04（裁决与信念账本）

**Status:** done — 2026-10-14（预授权执行；证据见 ## Comments）

**流程：** L2 详细设计 → owner 批准 → TDD 实现 → problem-log → 走查。

- [x] MetricsModule.feed(...) 唯一入口；report() 产出 divergence / ECE / 身份稳定性
- [x] 每个指标为内部可插拔实现（register_metric）；测试用一个假指标证明加指标零核心改动
- [x] ECE 判别测试：故意注入错误置信度的适配器必须被指标区分（Gate 1c 同构——pre-rebuild 的 0.9617 分离故事的复现）
- [x] divergence：真值位置 vs belief 主张按 entity_id 联表计算
- [x] 同输入 → 指标输出逐字节一致

## Comments

- 2026-10-14 · **TDD 红相位**：指标桩（空注册表）下 slice05 全失败（11.4s 超时线内 FAIL）；另修两处测试编译错（map const 访问用 .at）。
- 2026-10-14 · **绿相位（全量验证）**：`build.bat --test` → ctest **6/6 通过**（ow_slice05 0.02s）。
- 2026-10-14 · **证据（ECE 判别）**：诚实组（conf 0.875，acc 7/8）ece == 0.0；夸大组（conf 1.0 同错法）ece == 0.125——精确分离（全二进制精确路径）。Gate-1c 判别性的单元级复现。
- 2026-10-14 · **走查**：范围 = `src/metrics/`（deep module：一个 feed 入口 + 内部注册；替身指标注入零核心改动）+ tests/CMake/文书。
