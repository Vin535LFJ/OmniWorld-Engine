# 02: 观察生成

**What to build:** 从 WorldState 生成有界视图——任意请求分辨率的俯视 RGB + 帧元数据（backend/device/format/dims）+ 按实体三态可见性（看见/未见/不确定）。看不见的语义是"未见"，不是空也不是零（契约⑧）。

**Blocked by:** 01（真值心跳）

**Status:** design-review — L2 已产出待批：[docs/design/stage-a-core/slices/02-observation-generation.md](../../../docs/design/stage-a-core/slices/02-observation-generation.md)

**流程：** L2 详细设计 → owner 批准 → TDD 实现 → problem-log → 走查。

- [ ] render() 以**参数**接收分辨率——代码中不存在写死的分辨率常量（旧 224 bug 从设计上消灭的实证）
- [ ] 可见性三态：视野内 = Seen；视野外 = Unseen；遮挡/边界 = Uncertain——未见 ≠ 空值 ≠ 零
- [ ] FrameMeta 携带 backend/device/format/dims（参考模式如实标注 cpu/host/RGB8）
- [ ] 同 seed → 观察序列逐字节一致；非默认分辨率测试通过
- [ ] git diff 只触碰 observation 模块
