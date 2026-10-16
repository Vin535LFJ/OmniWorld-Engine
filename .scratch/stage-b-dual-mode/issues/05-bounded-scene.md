# 05: 有界场景（F2）

**What to build:** `bounds(none|wall)` 场景策略进 `RunConfig`（**无默认值**），由 LoopDriver 构造时注入 WorldState；`step()` 实现带边界推进（wall = 镜像反射 + 速度取反，常数边距 0.02 与默认 footprint 对齐并写明耦合）；补齐 clamp 与"单轴单 tick 恰一次反射"不变式测试；边界事件计数进 TickLog（契约⑨ 追加字段，L0 §2 注释已同步）。

**Blocked by:** 03（TickLog 结构面）

**Status:** done — 2026-10-16（预授权执行；证据见 ## Comments）

**流程：** L2 详细设计 → owner 批准 → TDD → problem-log → 走查。

- [x] `RunConfig.bounds` 无默认值（none|wall）；`WorldState` 构造注入（接口零改、friend 写面不动——L1 §4）
- [x] **none = 现行为逐字节不变**：用 2026-10-14 对齐报告参数复跑，`world_hash` 与其记录一致
- [x] wall = 镜像反射（x/z 各一次、无循环 clamp）；零 RNG（确定性论证入 L2）
- [x] 不变式测试：单轴单 tick 恰一次反射；clamp 注释与实现一致（修掉旧实现"注释写 clamp 但无 clamp"的已记录偏差）
- [x] `TickLog.boundary_events` 计数（追加字段、既有语义零改动）；wall 面板跑对齐场景，观察错分是否随生产者能力分离（供 F1 读数，如实报告）

## Comments

- 2026-10-16 · **TDD**：slice11（6 组）+ 全量 ctest **12/12 绿**（ow_slice11 0.19s）。关键断言：600/42 none 模式 → `world_hash == 574361e62eec92b9`（对齐基线字面量）——none 行为零变化。
- 2026-10-16 · **F2 墙场景重测判据 (c)**（problem-log 006 闭环）：600/42 `--bounds wall` → per-producer ECE：blob_detect 0.194123 / overconfident 0.010000 / **分离 0.184123 < 0.30**；boundary_events=234；双跑哈希一致（墙场景确定性 ✓）。如实结论：墙给世界模型制造了可学的结构化误差（234 次反射），但夸大检测器在合成场景下准确率≈0.98，0.99 自报反而近校准——旧阈值来自故障注入场景，本场景不触发该故障 → 判据 (c) 维持 PARTIAL（见 problem-log 007）。
- 2026-10-16 · **走查**：diff = `src/core/types.hpp`（BoundsPolicy 移入）+ `src/world_state/` + `src/loop/driver.{hpp,cpp}` + tests/CMake/文书——L2 §偏差记录三条（BoundsPolicy 位置、无默认构造、margin 默认层）全部如实。
- 2026-10-16 · 新坑：无（极端速度钳制与计时分辨率已在 L2 §5 边界表预置）。
