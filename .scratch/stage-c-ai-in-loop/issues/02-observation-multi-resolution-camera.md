# 02: Observation 多分辨率 + Camera 合并（C2 · 候选 #3）

**What to build:** Camera（TopDownCamera 的投影角色）正式并入 Observation——观察自带"相机与分辨率"；分辨率属性彻底化：真模型要求的分辨率（RT-DETRv2-S 640×640 / DA-V2-Small ~518×518）经 render() 参数产出，代码中无分辨率魔法常量（旧 224 bug 类从设计上消灭的收尾）。

**Blocked by:** 01（依赖隔离——C2 无 ORT 依赖但同属 provider 叙事的前置）

**Status:** done — 2026-10-16（预授权执行；证据见 ## Comments）

**流程：** 预授权执行（同 C1 授权链）。

- [x] Camera 并入 Observation（view_pose + 分辨率成为观察的组成部分；对外类型 TopDownCamera 的独立投影职责归位）
- [x] render() 按模型分辨率产出（640×640 / 518×518 实测通过，元数据如实标注）
- [x] 参考模式基准不变：600/42 场景 `world_hash == 574361e62eec92b9` / `ledger_hash == 459ff85132cc103e`（slice11 断言同款）
- [x] slice12 测试：多分辨率渲染 + 三态可见性 + 确定性成对
- [x] 设计文档同步：L1 §4 签名 + 候选 #3 在 PLAN 的条目标记完成

## Comments

- 2026-10-16 · **TDD**：slice12（5 组）一次即绿；全量 ctest **14/14 绿**（ow_slice12）。
- 2026-10-16 · **候选 #3 收尾**：`ViewCamera`（pose + 分辨率 + 投影职责）并入 Observation——观察自带相机；`TopDownCamera`/自由投影函数退场；FrameMeta dims 与相机单点构造、不可能不一致。
- 2026-10-16 · **基准不变证据**：600/42 默认场景 → `world_hash == 574361e62eec92b9` / `ledger_hash == 459ff85132cc103e`（字面量断言，Camera 并入零字节变化）。
- 2026-10-16 · **多分辨率实测**：32/64/518/640 四种分辨率（覆盖 DA-V2-Small ~518 与 RT-DETRv2-S 640 输入）渲染通过、实体足迹落点随分辨率比例映射；三态判定为几何谓词、与分辨率无关（实测三种分辨率下一致）。
- 2026-10-16 · 编译坑（自查捕获）：头文件内 `std::floor` 缺 `<cmath>` 包含——inline 函数进头文件的教科书级陷阱。
- 2026-10-16 · 新坑：无。
