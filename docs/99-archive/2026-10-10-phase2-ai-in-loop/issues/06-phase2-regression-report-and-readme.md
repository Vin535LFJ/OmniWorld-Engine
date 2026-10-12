# 06: Phase 2 收口：regression 报告 + README 双模式文档

**What to build:** 把 04/05 的数据收口成 Phase 2 出口要求的完整 regression 报告（完整环境元数据 + 模式标注 + 延迟分项 + 真模型 vs 内置对比），并更新 README（中英）：性能模式构建/运行说明 + 模型获取方式文档；无 ONNX 环境的用户 clone 即跑参考模式的路径保持不变。这是作品集 Gate（ADR-0008）的第一块公开工件。

**Blocked by:** 04、05

**Status:** ready-for-agent

- [ ] Phase 2 regression 报告落 `benchmarks/regression/`（格式对齐 2026-09-28 两份：环境元数据 + 可复现命令 + 模式标注）
- [ ] README 中英双版含性能模式说明与模型获取文档；参考模式零依赖路径不变（`build.bat --test` 即验证）
- [ ] 对照 CURRENT.md 出口条件逐条自查：真模型单文件接入核心零改动 / 参考模式位级复现保持 / 性能模式延迟预算报告——未达标项如实标注，不粉饰
- [ ] 最终全量验证：ctest 全绿 + 参考模式逐字节复现
