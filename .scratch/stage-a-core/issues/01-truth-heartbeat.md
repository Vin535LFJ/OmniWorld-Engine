# 01: 真值心跳

**What to build:** 一个实体在 tick 推进下确定地动起来——truth_world 确定性运动学推进 WorldState，同 seed 同输入逐字节复现。这是整个引擎的心跳：先让世界活着，再谈谁来看它。

**Blocked by:** None（can start immediately）

**Status:** done

**流程：** 第一步产出本切片的 L2 详细设计（`docs/design/stage-a-core/slices/01-truth-heartbeat.md`：接口签名细化/数据结构/算法/边界情形/流程图/测试计划）→ owner 批准 → TDD 实现 → problem-log → 走查。

- [x] truth_world 按契约③（tick 单调时钟域）确定性推进 WorldState；同 seed → 逐字节一致
- [x] WorldState 只读查询面（find/tick），**无任何公开写接口**（编译级保证：写通道仅构造注入对 truth_world 可见，契约④⑤的结构性兑现）
- [x] core/types 公共值类型就位（Tick/EntityId/Vec3/Quat/Transform/Visibility/ClassTag）
- [x] 新增测试全绿（`build.bat --test`）；MSVC 浮点选项按 L1 §6 固定
- [x] git diff 证明本切片只触碰 L1 §5 声明的模块（core/types, truth_world, world_state）

## Comments

- 2026-10-13 · 完成。证据：`CURRENT.md`（切片 01 已完成，ctest 2/2 绿，`ow_slice01` 覆盖 5 组行为）；提交 `de8d512`（L2 + TDD 红→绿）与 `3011cd0`（verified + C++17 降级实验）。
- 勾选依据为上述仓库记录，**非本次复核的实测结果**；如需重新取证，跑 `build.bat --test`。
