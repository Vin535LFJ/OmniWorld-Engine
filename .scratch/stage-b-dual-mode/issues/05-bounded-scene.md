# 05: 有界场景（F2）

**What to build:** `bounds(none|wall)` 场景策略进 `RunConfig`（**无默认值**），由 LoopDriver 构造时注入 WorldState；`step()` 实现带边界推进（wall = 镜像反射 + 速度取反，常数边距 0.02 与默认 footprint 对齐并写明耦合）；补齐 clamp 与"单轴单 tick 恰一次反射"不变式测试；边界事件计数进 TickLog（契约⑨ 追加字段，L0 §2 注释已同步）。

**Blocked by:** 03（TickLog 结构面）

**Status:** ready-for-agent

**流程：** L2 详细设计 → owner 批准 → TDD → problem-log → 走查。

- [ ] `RunConfig.bounds` 无默认值（none|wall）；`WorldState` 构造注入（接口零改、friend 写面不动——L1 §4）
- [ ] **none = 现行为逐字节不变**：用 2026-10-14 对齐报告参数复跑，`world_hash` 与其记录一致
- [ ] wall = 镜像反射（x/z 各一次、无循环 clamp）；零 RNG（确定性论证入 L2）
- [ ] 不变式测试：单轴单 tick 恰一次反射；clamp 注释与实现一致（修掉旧实现"注释写 clamp 但无 clamp"的已记录偏差）
- [ ] `TickLog.boundary_events` 计数（追加字段、既有语义零改动）；wall 面板跑对齐场景，观察错分是否随生产者能力分离（供 F1 读数，如实报告）
