# 05: 判据 (c) 重测 + 阶段 C 收口报告（C5）

**What to build:** 用真实模型场景重测判据 (c)：至少一个"接近校准”的真模型 vs 夸大置信度模型的 per-producer ECE 分离量（vs 0.30 旧阈值）——如实裁定达标/未达标；产出阶段 C regression 报告（环境元数据 + 模式标注 + 延迟分项 + 模型清单）。

**Blocked by:** 03、04（双真模型在场才有面板）

**Status:** ready-for-agent

**流程：** 预授权执行（同 C1 授权链）。

- [ ] per-producer ECE 分离量实测（真模型 vs 夸大；对照 0.30 阈值）
- [ ] regression 报告落 `benchmarks/regression/`（环境元数据 + mode + 延迟 p50/p95 + 模型清单与许可）
- [ ] 判据 (c) 裁定：达标 → 更新 PLAN/问题日志；未达标 → 如实 PARTIAL + 关闭路径
- [ ] 阶段 C 出口对照（PLAN §六 Phase 2 行）逐条自查；CURRENT → 阶段收口/下一阶段
