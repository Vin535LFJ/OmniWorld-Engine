# 02: ONNX Runtime 可行性核验（research note）

**What to build:** 解 Phase 2 出口所需的全部外部事实（遵循 PLAN 规则 #3：研究只解当前 phase 出口的问题）：Windows 下 ONNX Runtime CUDA EP 的版本/dll/依赖核验（RTX 4060 8GB 实测可用性）；RT-DETRv2-S 与 Depth-Anything-V2-Small 的 ONNX 导出/获取方式与许可溯源（Apache-2.0；DepthAnything 仅 Small 可商用）；两模型的输入/输出张量形状与预处理契约；模型文件的存放与获取决策（下载脚本 vs 仓库内）。产出一篇带一手来源引用的 research note 落 `docs/17-research/`。

**Blocked by:** None（can start immediately；建议与 01 并行）

**Status:** ready-for-agent

- [ ] research note 落库：ONNX Runtime CUDA EP 版本矩阵 + 4060 8GB 可用性结论（引用官方一手来源，不转述博客）
- [ ] 两模型的获取/导出路径与许可结论明确（引用许可证原文；许可红线见 PLAN §五）
- [ ] 模型文件存放/获取方式有明确决策及理由，供 04/05 执行
- [ ] 若发现阻碍性事实（如 CUDA EP 在本机不可用），如实记录替代路径（如 CPU EP 先行）并在 ticket 04 里反映，不沉默绕过
