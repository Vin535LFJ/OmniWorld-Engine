# PLAN — OmniWorld 唯一必读文档

> 这是 2026-09-28 文档瘦身后的**唯一活跃计划文件**。读完这一篇 = 读完以前 vision/roadmap/sprint/world-state/policy 的全部必要内容。其余历史文档全部在 `docs/99-archive/`，只查不读。
> 快照：2026-09-28（方向决策 2026-09-21；同日补入想法族谱与终局画面）。有且只有本文件 + `CURRENT.md`（本周）+ `README.md`（门面）三份活跃文档。

---

## 一、这个项目是什么（3 句话）

1. **OmniWorld 是一个开放运行时：让感知模型、世界模型、仿真器、渲染器、智能体共享同一个"活着的世界状态"**——每个实体有稳定身份、每个读写有时间戳、每个字段有归属、每个判断带置信度。
2. 它存在的原因：大公司都在造"生成世界的 AI 模型"，但没有人提供让这些模型与真值仿真器互相校准的**开放中立工具**；而十几个评测学术团队每一个都在重复手写同一套闭环胶水（WorldMark 原话："no shared protocol has been established"）。
3. 第一站是**评测**：做一个任何人都能把模型插进来、跑闭环、出可复现指标的 harness。它不是游戏引擎、不是大模型、不做工业数字孪生（那是 NVIDIA Mega 的领地，正面避开）。

**与旧蓝图的关系**：2026-08 的原始蓝图（归档《GPU 原生实时 AI 世界引擎…报告》）写的是*"从传感器视频解码、神经网络推理到 3D 实时渲染与 VLA 具身智能闭环的显存零拷贝统一计算平台"*——**本项目的终局就是这张图**（见第三节）；收敛改变的只是建造顺序，不是终点。

## 二、想法族谱（你曾有的 40 项构想 → 六个去处，全部带回归条件）

> 2026-09-28 复盘补录：收敛没有删除任何想法，只是给它们排序、找支点。每个去处都有明确的回归规则。

| 去处 | 想法（原词） | 回归条件 |
|---|---|---|
| **① 支点 = 项目本体（活着）** | WorldState/Observation/Action/Transition 契约；"真实+合成世界统一"（grounding loop 正是合成半边，真实半边由视频适配器回归）；显存零拷贝纪律（化为 9 字段 buffer 契约 + per-stage 延迟计量——最初的技术论点就是今天的工程脊柱）；回放数据输入 | 就是现在，Week 2 起 |
| **② Adapter 归位（绕着支点长回来）** | NVDEC/NVENC/FFmpeg 视频管线；TensorRT（ONNX Runtime 先行顶位）；CV-CUDA 预处理；CARLA/Isaac 仿真接入；ROS 2/NITROS（≥6 个月）；Jolt 物理（Phase 3）；Brain/LLM/VLA/WAM（Phase 2 起从最小 Agent 契约生长） | "某 phase 的 benchmark 需要它"即回归 |
| **③ 技能项目（学习 + 作品集，不进主线承诺）** | Vulkan 1.3 离屏渲染器与效果栈：PBR、**Post-Process Pass（记忆中"特效"的真身）**、光追、3DGS/神经渲染；CUDA 深度优化（自定义 GEMM/WMMA） | Gate 1 后缓冲期可选立项；首选候选：离屏 Vulkan 观察渲染器替换 CPU 光栅器 |
| **④ 被 Track B 吸收（职业构想的现代形态）** | 十级专家阶梯；求职矩阵（NVIDIA/Tesla/Figure/宇树…）；GitHub 1k stars；CPU vs GPU 展示 demo（"80% vs 3%"） | Track B 本身：每 Phase 对外工件 + 被 merge 的 PR——比攒 stars 更可执行 |
| **⑤ 明确让渡（有证据地不做）** | 数字孪生/工业（NVIDIA Mega 领地）；游戏引擎化/编辑器；分布式/移动端/Jetson；AD/ADAS 作为产品方向（评测 harness 仍可服务 AD 数据） | 证据变化才复议（Gate/ADR） |
| **⑥ 记忆澄清** | "特效/粒子/VFX"：仓库从未单独立项；来源=渲染效果栈（③）+ 收藏的粒子系统教程。不是记错，是从未命名——现在族谱里有了 | — |

另：`knowledge/` 的 20 个主题目录（cuda/vulkan/rendering/robotics/world-model…）原样保留——旧技能矩阵的活地图，随时可学，与主线不冲突。

## 三、终局画面（要做成什么样）

**原图即终局，变的只是顺序：旧 Phase 0–8 自底向上先修路（世界排第 5 位，路永远修不完）；新路线从心脏（World State）先活起来，其余一切作为适配器绕着它长回来。**

```text
       视频/相机(NVDEC) ─┐
       感知模型(ONNX/TRT) ─┤
       渲染(Vulkan) ──────┤   ← 全是 Adapter，按证据回归（族谱②③）
       仿真(Jolt/CARLA) ──┤
       Brain/VLA ────────┘
                ↓ 全部接入同一个
        [Canonical World State]   ← Core：tick / 身份 / 所有权 / 置信度
                ↓
        Observation → 闭环指标 → 可复现报告
```

- **1 年后的具体样子**：`omniworld run --adapter my_model.onnx --world scenes/mini_city.toml` → 输出闭环报告（30Hz、身份稳定性、预测偏差、延迟分解）；一份被引用的 benchmark 笔记；一个被 merge 的 WorldMark 适配器 PR；陌生人 clone 即跑。
- **5 年后（North Star）**：评测交互式世界模型时的默认接入路径——MLPerf 之于测量；此时 ②③ 组的适配器都已绕着心脏长成。

## 四、双轨目标（2026-09-24 你确认）

- **Track A 生态采用**：评测社区真的用起来（外部 issue/引用/合作）。
- **Track B 个人技能与敲门砖**：主线产出的公开工件（benchmark 报告、可跑 harness、被 merge 的 OSS PR、工程笔记）本身就是进入 NVIDIA/World Labs/DeepMind/HF 等团队的作品集。
- **规则**：Track B 不能替代技术判决——下面的 Gate 1 该失败就失败。

## 五、范围四分

| 层 | 内容 |
|---|---|
| **CORE（自写）** | World Contract v0（第七节）、闭环 runtime（tick/版本/所有权裁决）、grounding 指标（身份稳定性/位置误差/时间一致性/帧率）、确定性真值世界（~2 页积分器）、离屏 CPU 光栅器（~200 行）、实体关联（~200 行） |
| **ADAPTER（引用）** | ONNX Runtime（MIT，NuGet）、RT-DETRv2-S 检测（Apache-2.0）、Depth-Anything-V2-Small 深度（Apache-2.0，仅 Small 可商用）、MCAP 录制（MIT）、回放数据、Jolt 物理备选（MIT）、ByteTrack-cpp 只读参考（MIT） |
| **EXPLORATION** | OpenUSD 单向导出、冲突策略族、不确定性传播、第三方开源世界模型接入（若 8GB 能跑）、离屏 Vulkan 观察渲染器（Gate 1 后技能项目首选） |
| **BACKLOG（不做）** | 训练任何模型、ROS 2/VLA/LLM、分布式、游戏 demo、编辑器、商业化、OpenUSD 全兼容、Vulkan 完整渲染器、移动端 |

**许可红线**：AGPL（Ultralytics YOLO 全系/FastSAM/boxmot）、CC-BY-NC（DepthAnything 非 Small）、无许可证（tinyrenderer）一律不进产品路径；vendored 代码放 `third_party/` 并保留原许可证。完整规则见归档 `third-party-policy.md`。

## 六、12 个月路线图与三个 Gate（假设 ~20h/周）

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

## 七、World Contract v0（写代码时的唯一依据）

**设计规则**：每个字段必须被 30 天代码用到，没消费者就删；只加字段不改语义；语义变更 = 新版本 + ADR。

**Entity**：`entity_id`(u64，runtime 铸造不复用)、`class_tag`(字符串)、`transform`(pos+quat，单一世界系)、`velocity`、`owner`(组件 id，唯一写入者)、`source`(`ground_truth`|`belief`)、`updated_at`(tick)。

**Observation**：`tick`、`view_transform`(观察者位姿——v0 唯一允许的第二坐标系)、`sensor_kind`(`rgb`)、帧 buffer 元数据（backend/device/format/dims/pitch/lifetime/sync/timestamp，九字段继承自 memory 契约）。

**Proposal（非拥有者的唯一写路径）**：`producer`、`tick_observed`、`target`(entity_id 或 new_entity)、`fields`(提议值)、`confidence`([0,1])。

**WorldTransition（每 tick 日志）**：`tick`、`state_version`、`applied`/`rejected`(含拒绝原因)、`per_stage_latency`。

**Action**：`target`、`action_type`+payload、`issued_by`、`tick`（只验证 schema 与所有权，语义在适配器层）。

**六条语义**：① 身份由 runtime 铸造与裁决（v0 = 最近变换+类别匹配，可替换）；② 单一 tick 时钟域，wall-clock 仅诊断用；③ 真值独占运动学字段，模型只能写 belief 层 proposal——指标 = (truth, belief) 按 entity_id 联表；④ v0 冲突策略只有"真值赢、beliefs 留痕"；⑤ 置信度单浮点，校准问题体现为指标而非静默覆盖；⑥ 部分观测带显式视锥元数据，未见 ≠ 空。

**明确不进 v0**：多坐标系树、分布式状态、概率分布字段、语义层级/关系、生成式表示共存、OpenUSD 导出、显示渲染（v0 离屏）。

**v0.1 冻结（2026-09-28，依 Phase 0/1 实测）**：语义零改动；补充 as-built 事实——① 第二类参与者"世界模型适配器"（`predict(t, entities) → proposals`，`target_hint` 作为指标侧证据）；② 回放格式 rollout.txt（tab 分隔 tick/id/class/x/z/vx/vz）；③ 评测中世界模型 confidence 固定 0.5，逐模型置信度估计为 v0.2 工作；④ belief 层只记录不改动拥有字段（已由测试守护）。证据：两份 regression 报告。

## 八、前 30 天（执行状态：Phase 0 + Phase 1 已于 2026-09-28 完成）

- **Week 1（已完成）**：方向落库 + 代码保真 + 归档。
- **Week 2–4（已完成，一次通过）**：`src/world_state` + 确定性真值世界 + 离屏光栅 + 位级确定性测试 + 双感知适配器 + 裁决 + 指标 + **Gate 1 三判据全 PASS**（1106.7 Hz / 适配器零核心改动 / ECE 判别分离 0.9617）→ [`benchmarks/regression/2026-09-28-grounding-loop-v0.md`](benchmarks/regression/2026-09-28-grounding-loop-v0.md)。
- **Phase 1（已完成）**：回放录制 + replay/linear/zero/mlp/damped_linear 五模型经同一契约入闭环，divergence 五级分离；第五模型以**单个新文件**接入（核心零改动实证）→ [`benchmarks/regression/2026-09-28-phase1-world-models.md`](benchmarks/regression/2026-09-28-phase1-world-models.md)。
- **下一步 = Phase 2**（第 4–6 月项，见 §6）：3+ 第三方可评测对象、WorldMark/WBench 动作词汇对齐、公开 v0.1 发布 + 首个 OSS PR。

## 九、操作规则

1. 新想法 → `docs/17-research/research-queue.md`，不进 sprint；sprint 内只执行不扩写。
2. 没有 benchmark 数据不宣称性能；报告必须带完整环境元数据（参照 2026-09-18 baseline 格式）。
3. 研究只解当前 phase 出口所需的问题；产 research note 带来源。
4. **代码未跑通前不加新文档。**

## 十、证据与深度阅读（想查才查，不必读）

- 方向证据（为什么是这条路，203 行 + 来源）：`docs/17-research/2026-09-21-direction-decision.md`
- 复用与选型依据（18 仓库调研 + 敲门砖贡献目标）：`docs/17-research/2026-09-24-github-reuse-map.md`
- 待研究问题与盯防清单：`docs/17-research/research-queue.md`
- 历史文档（旧规划/原蓝图报告/讨论记录/导读系列/生命周期/模板）：`docs/99-archive/`
