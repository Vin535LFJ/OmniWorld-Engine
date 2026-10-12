# 04: RT-DETRv2-S 真模型入环

**What to build:** 第一个真实 AI 在环——RT-DETRv2-S 经 ONNX Runtime 以单文件 Perception Adapter 接入 Grounding Loop：预处理归适配器所有（按模型契约 resize/normalize），输出映射为 Proposal（置信度字段语义不变）；性能模式跑 CUDA EP，参考模式路径不动。适配器仅在 ONNX Runtime 存在时编译（build flag，核心零改动）。产真模型 vs 内置检测器（blob_cc/blob_grid）的对比数据：身份稳定性 / Divergence / per-stage 延迟分项。

**Blocked by:** 01（分辨率属性化）、02（模型来源与 ORT 事实）、03（双模式框架）

**Status:** ready-for-agent

- [ ] RT-DETRv2-S 以单文件适配器接入，核心零 diff（适配器不变式由 git diff 实证——damped_linear 先例）
- [ ] 参考模式（默认）ctest 全绿、同 seed 逐字节复现保持
- [ ] 性能模式：固定 seed 容差复现通过 + 4060 8GB 延迟预算实测（preprocess / inference / postprocess / adjudication 分项，模式 + 环境元数据标注）
- [ ] 真模型 vs 内置检测器的指标对比数据落 regression 报告 draft（06 票收口）
