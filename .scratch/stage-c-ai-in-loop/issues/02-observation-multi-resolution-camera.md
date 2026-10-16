# 02: Observation 多分辨率 + Camera 合并（C2 · 候选 #3）

**What to build:** Camera（TopDownCamera 的投影角色）正式并入 Observation——观察自带"相机与分辨率"；分辨率属性彻底化：真模型要求的分辨率（RT-DETRv2-S 640×640 / DA-V2-Small ~518×518）经 render() 参数产出，代码中无分辨率魔法常量（旧 224 bug 类从设计上消灭的收尾）。

**Blocked by:** 01（依赖隔离——C2 无 ORT 依赖但同属 provider 叙事的前置）

**Status:** ready-for-agent

**流程：** 预授权执行（同 C1 授权链）。

- [ ] Camera 并入 Observation（view_pose + 分辨率成为观察的组成部分；对外类型 TopDownCamera 的独立投影职责归位）
- [ ] render() 按模型分辨率产出（640×640 / 518×518 实测通过，元数据如实标注）
- [ ] 参考模式基准不变：600/42 场景 `world_hash == 574361e62eec92b9` / `ledger_hash == 459ff85132cc103e`（slice11 断言同款）
- [ ] slice12 测试：多分辨率渲染 + 三态可见性 + 确定性成对
- [ ] 设计文档同步：L1 §4 签名 + 候选 #3 在 PLAN 的条目标记完成
