# 03: RT-DETRv2-S 适配器（C3）

**What to build:** 第一个真实 AI 在环——RT-DETRv2-S 以单文件 Perception Adapter 接入，内部双 provider（CPU EP 参考路径 / CUDA EP 性能路径，`--ep` 运行时选择）；检测框 → Proposal 五元组映射（target 用 NewEntity + transform + confidence 按置信度）；性能模式容差窗口声明（ADR-0009:16，原则 = 实测最大抖动 × 2）。

**Blocked by:** 01（ORT 依赖隔离）、02（多分辨率 Observation——模型输入 640×640）

**Status:** ready-for-agent

**流程：** 预授权执行（同 C1 授权链）。

- [ ] 单文件适配器（`src/perception/rtdetr.cpp` 类）+ 编译期条件（ORT 存在才编）；核心零改动
- [ ] 参考路径（CPU EP）：逐字节复现保持；性能路径（CUDA EP）：容差窗口声明 + 延迟预算（由 C5 报告实测）
- [ ] 检测框 → Proposal 映射（类别标签、位置、置信度）经裁决入信念账本
- [ ] git diff = 单文件 + build/文书（适配器不变式的 git 证据）
- [ ] slice13 测试：参考路径确定性 + 性能路径容差 + 单文件不变式
