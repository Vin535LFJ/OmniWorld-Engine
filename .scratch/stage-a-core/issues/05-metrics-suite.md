# 05: 指标组

**What to build:** Metrics 深模块——一个 feed 入口（真值 + 信念账本 + Tick 日志），divergence / ECE / 身份稳定性在内部可插拔注册；加新指标不动核心。

**Blocked by:** 04（裁决与信念账本）

**Status:** ready-for-agent

**流程：** L2 详细设计 → owner 批准 → TDD 实现 → problem-log → 走查。

- [ ] MetricsModule.feed(...) 唯一入口；report() 产出 divergence / ECE / 身份稳定性
- [ ] 每个指标为内部可插拔实现（register_metric）；测试用一个假指标证明加指标零核心改动
- [ ] ECE 判别测试：故意注入错误置信度的适配器必须被指标区分（Gate 1c 同构——pre-rebuild 的 0.9617 分离故事的复现）
- [ ] divergence：真值位置 vs belief 主张按 entity_id 联表计算
- [ ] 同输入 → 指标输出逐字节一致
