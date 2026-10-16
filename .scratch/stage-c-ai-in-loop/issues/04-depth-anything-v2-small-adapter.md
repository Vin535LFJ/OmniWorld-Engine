# 04: Depth-Anything-V2-Small 适配器（C4）

**What to build:** 第二个真实模型家族（深度估计）经同一接缝单文件接入——证明契约家族无关（检测 + 深度两类参与者走同一条路）；深度图 → 观测增强或 Proposal 语义的映射设计（若超出 v0.1 冻结语义 → 升 ADR 走 v0.2，不硬改）。

**Blocked by:** 03（RT-DETRv2-S 模式先立）

**Status:** ready-for-agent

**流程：** 预授权执行（同 C1 授权链）。

- [ ] DA-V2-Small 单文件适配器接入；核心零改动
- [ ] 深度输出的契约映射设计（在三态可见性 / Proposal 语义内解决；需语义扩展则走 ADR）
- [ ] 容差窗口声明 + 延迟预算路径同 C3
- [ ] git diff = 单文件 + build/文书
- [ ] slice14 测试：映射行为 + 确定性 + 容差
