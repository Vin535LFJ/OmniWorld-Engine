# PLAN — OmniWorld 唯一必读文档

> 这是 2026-09-28 文档瘦身后的**唯一活跃计划文件**。读完这一篇 = 读完以前 vision/roadmap/sprint/world-state/policy 的全部必要内容。其余历史文档全部在 `docs/99-archive/`，只查不读。
> 快照：2026-09-28（方向决策 2026-09-21）。有且只有本文件 + `CURRENT.md`（本周）+ `README.md`（门面）三份活跃文档。

---

## 一、这个项目是什么（3 句话）

1. **OmniWorld 是一个开放运行时：让感知模型、世界模型、仿真器、渲染器、智能体共享同一个"活着的世界状态"**——每个实体有稳定身份、每个读写有时间戳、每个字段有归属、每个判断带置信度。
2. 它存在的原因：大公司都在造"生成世界的 AI 模型"，但没有人提供让这些模型与真值仿真器互相校准的**开放中立工具**；而十几个评测学术团队每一个都在重复手写同一套闭环胶水（WorldMark 原话："no shared protocol has been established"）。
3. 第一站是**评测**：做一个任何人都能把模型插进来、跑闭环、出可复现指标的 harness。它不是游戏引擎、不是大模型、不做工业数字孪生（那是 NVIDIA Mega 的领地，正面避开）。

## 二、双轨目标（2026-09-24 你确认）

- **Track A 生态采用**：评测社区真的用起来（外部 issue/引用/合作）。
- **Track B 个人技能与敲门砖**：主线产出的公开工件（benchmark 报告、可跑 harness、被 merge 的 OSS PR、工程笔记）本身就是进入 NVIDIA/World Labs/DeepMind/HF 等团队的作品集。
- **规则**：Track B 不能替代技术判决——下面的 Gate 1 该失败就失败。

## 三、范围四分

| 层 | 内容 |
|---|---|
| **CORE（自写）** | World Contract v0（下文第五节）、闭环 runtime（tick/版本/所有权裁决）、grounding 指标（身份稳定性/位置误差/时间一致性/帧率）、确定性真值世界（~2 页积分器）、离屏 CPU 光栅器（~200 行）、实体关联（~200 行） |
| **ADAPTER（引用）** | ONNX Runtime（MIT，NuGet）、RT-DETRv2-S 检测（Apache-2.0）、Depth-Anything-V2-Small 深度（Apache-2.0，仅 Small 可商用）、MCAP 录制（MIT）、回放数据、Jolt 物理备选（MIT）、ByteTrack-cpp 只读参考（MIT） |
| **EXPLORATION** | OpenUSD 单向导出、冲突策略族、不确定性传播、第三方开源世界模型接入（若 8GB 能跑） |
| **BACKLOG（不做）** | 训练任何模型、ROS 2/VLA/LLM、分布式、游戏 demo、编辑器、商业化、OpenUSD 全兼容、Vulkan 完整渲染器、移动端 |

**许可红线**：AGPL（Ultralytics YOLO 全系/FastSAM/boxmot）、CC-BY-NC（DepthAnything 非 Small）、无许可证（tinyrenderer）一律不进产品路径；vendored 代码放 `third_party/` 并保留原许可证。完整规则见归档 `third-party-policy.md`。

## 四、12 个月路线图与三个 Gate（假设 ~20h/周）

| Phase | 时间 | 目标 | 出口 | 对外工件 |
|---|---|---|---|---|
| **0 Grounding Loop v0** | 第 1 月 | 闭环跑通：真值世界+双感知模型+指标 | Gate 1 | Gate-1 regression 报告 |
| **1 世界模型接入** | 第 2-3 月 | 回放数据+小 dynamics 模型进闭环；契约 v0.1 | 新模型只写 1 个适配器文件；首份对外 benchmark 笔记 | benchmark 笔记 |
| **2 对外发布 v0.1** | 第 4-6 月 | 3+ 个可评测对象；对齐 WorldMark/WBench 词汇 | **Gate 2** | harness 公开发布 + 首个被 merge 的 OSS PR |
| **3 契约硬化** | 第 7-9 月 | 冲突策略族、外部物理引擎、第二个真值世界 | 指标跨世界可复现 | 契约硬化工程笔记 |
| **4 分发与生态** | 第 10-12 月 | 打包、适配器示例、可投稿笔记 | **Gate 3** | 可运行的发布版 |

**Gate 1（第 30 天，纯技术，不可双轨）**：① 闭环端到端 ≥30Hz（RTX 4060）；② 第二个感知模型接入 = 只改适配器文件、核心零改动；③ 指标能区分正常模型 vs 故意注入错误置信度的模型。失败 → 缩小 MVP 或修契约，不允许"失败但继续写"。
**Gate 2（第 6 月，双轨）**：外部兴趣信号 **或** 敲门砖证据（PR 被 merge/合作邀请/面试级演示），任一即过；两者皆无 → 降级为个人研究仪器，停止平台化投入。
**Gate 3（第 12 月，双轨）**：≥2 个真实外部使用 **或** 持续敲门砖进展；否则冻结扩张，转维护态。

## 五、World Contract v0（写代码时的唯一依据）

**设计规则**：每个字段必须被 30 天代码用到，没消费者就删；只加字段不改语义；语义变更 = 新版本 + ADR。

**Entity**：`entity_id`(u64，runtime 铸造不复用)、`class_tag`(字符串)、`transform`(pos+quat，单一世界系)、`velocity`、`owner`(组件 id，唯一写入者)、`source`(`ground_truth`|`belief`)、`updated_at`(tick)。

**Observation**：`tick`、`view_transform`(观察者位姿——v0 唯一允许的第二坐标系)、`sensor_kind`(`rgb`)、帧 buffer 元数据（backend/device/format/dims/pitch/lifetime/sync/timestamp，九字段继承自 memory 契约）。

**Proposal（非拥有者的唯一写路径）**：`producer`、`tick_observed`、`target`(entity_id 或 new_entity)、`fields`(提议值)、`confidence`([0,1])。

**WorldTransition（每 tick 日志）**：`tick`、`state_version`、`applied`/`rejected`(含拒绝原因)、`per_stage_latency`。

**Action**：`target`、`action_type`+payload、`issued_by`、`tick`（只验证 schema 与所有权，语义在适配器层）。

**六条语义**：① 身份由 runtime 铸造与裁决（v0 = 最近变换+类别匹配，可替换）；② 单一 tick 时钟域，wall-clock 仅诊断用；③ 真值独占运动学字段，模型只能写 belief 层 proposal——指标 = (truth, belief) 按 entity_id 联表；④ v0 冲突策略只有"真值赢、beliefs 留痕"；⑤ 置信度单浮点，校准问题体现为指标而非静默覆盖；⑥ 部分观测带显式视锥元数据，未见 ≠ 空。

**明确不进 v0**：多坐标系树、分布式状态、概率分布字段、语义层级/关系、生成式表示共存、OpenUSD 导出、显示渲染（v0 离屏）。

## 六、前 30 天（详细版见归档 current-sprint.md）

- **Week 1（已完成）**：方向落库 + 代码保真 + 归档。
- **Week 2（下一步）**：`src/world_state`（实体注册表/tick/版本/transition 日志）+ 确定性真值世界（N 刚体定种子运动学）+ 离屏 Observation + 确定性测试（同种子逐字节一致，进 ctest）。出口：位级一致 + Observation ≥60Hz。
- **Week 3**：两个 ONNX 感知模型适配器 + ID 关联 + 所有权裁决 + 首批指标；故意坏掉的模型必须能被指标抓出。
- **Week 4**：完整 benchmark（per-stage p50/p95、端到端帧率、适配器不变式测试）+ regression 报告 + Gate 1 判定 + 工程笔记草稿（敲门砖工件第一件）。

## 七、操作规则

1. 新想法 → `docs/17-research/research-queue.md`，不进 sprint；sprint 内只执行不扩写。
2. 没有 benchmark 数据不宣称性能；报告必须带完整环境元数据（参照 2026-09-18 baseline 格式）。
3. 研究只解当前 phase 出口所需的问题；产 research note 带来源。
4. **代码未跑通前不加新文档。**

## 八、证据与深度阅读（想查才查，不必读）

- 方向证据（为什么是这条路，203 行 + 来源）：`docs/17-research/2026-09-21-direction-decision.md`
- 复用与选型依据（18 仓库调研 + 敲门砖贡献目标）：`docs/17-research/2026-09-24-github-reuse-map.md`
- 待研究问题与盯防清单：`docs/17-research/research-queue.md`
- 历史文档（旧规划/导读系列/生命周期/模板/两份旧研究报告修正记录）：`docs/99-archive/`
