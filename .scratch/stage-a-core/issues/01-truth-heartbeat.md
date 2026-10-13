# 01: 真值心跳

**What to build:** 一个实体在 tick 推进下确定地动起来——truth_world 确定性运动学推进 WorldState，同 seed 同输入逐字节复现。这是整个引擎的心跳：先让世界活着，再谈谁来看它。

**Blocked by:** None（can start immediately）

**Status:** ready-for-agent

**流程：** 第一步产出本切片的 L2 详细设计（`docs/design/stage-a-core/slices/01-truth-heartbeat.md`：接口签名细化/数据结构/算法/边界情形/流程图/测试计划）→ owner 批准 → TDD 实现 → problem-log → 走查。

- [ ] truth_world 按契约③（tick 单调时钟域）确定性推进 WorldState；同 seed → 逐字节一致
- [ ] WorldState 只读查询面（find/tick），**无任何公开写接口**（编译级保证：写通道仅构造注入对 truth_world 可见，契约④⑤的结构性兑现）
- [ ] core/types 公共值类型就位（Tick/EntityId/Vec3/Quat/Transform/Visibility/ClassTag）
- [ ] 新增测试全绿（`build.bat --test`）；MSVC 浮点选项按 L1 §6 固定
- [ ] git diff 证明本切片只触碰 L1 §5 声明的模块（core/types, truth_world, world_state）
