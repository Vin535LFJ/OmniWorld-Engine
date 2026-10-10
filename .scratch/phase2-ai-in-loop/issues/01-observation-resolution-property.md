# 01: Observation 分辨率属性化（prefactor，折入架构候选 #3）

**What to build:** Camera 并入 Observation 结构，分辨率从硬编码常量变成 Observation 的属性；默认 224×224 保持不变，参考模式逐字节回归不动。适配器从此可以按模型需求拿到任意分辨率的帧——为真模型入环扫清道路（"make the change easy, then make the easy change"）。设计理由与 224 潜伏 bug 的位置见 `docs/17-research/2026-10-09-architecture-review.html` 候选 #3。

**Blocked by:** None（can start immediately）

**Status:** ready-for-agent

- [ ] Observation 携带分辨率属性，代码中不再有 224 魔法常量（Camera 结构并入 Observation，架构候选 #3 完成）
- [ ] 默认分辨率下参考模式逐字节回归保持（ctest 全绿，Gate-1 复现逐字节一致）
- [ ] 新增一个构造非 224 分辨率 Observation 的测试并通过
- [ ] 核心零语义变化：World Contract v0.1 字段语义不动；若发现确需语义变更，停下升 ADR 走程序，不硬改
