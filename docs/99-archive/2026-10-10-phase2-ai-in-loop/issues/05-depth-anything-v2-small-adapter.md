# 05: Depth-Anything-V2-Small 第二家族适配器

**What to build:** 第二个真实模型家族（深度）入环，证明 Perception Adapter 契约家族无关。深度输出在 World Contract v0.1 冻结语义内找到最小映射并写清文档——**若确需新字段/新语义，停下升 ADR 走 v0.2 程序，不硬改**（PLAN §七规则：语义变更 = 新版本 + ADR）。声明本模型自己的容差窗口（ADR-0009 后果：每个 GPU 组件声明自己的复现契约），扩展真模型 vs 内置的对比数据。

**Blocked by:** 04（适配器模式与 build 接线）

**Status:** ready-for-agent

- [ ] Depth-Anything-V2-Small 以单文件适配器接入，核心零 diff
- [ ] 深度输出的契约映射有明确文档（或触发 ADR 语义变更程序，如实记录）
- [ ] 自有容差窗口声明 + 性能模式实测通过（容差 + 延迟预算）
- [ ] 检测 vs 深度两家族的对比数据落 regression 报告 draft
