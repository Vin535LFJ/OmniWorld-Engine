# PLAN — OmniWorld 唯一必读文档

> 2026-10-10 方向重开重构（[ADR-0008](docs/adr/0008-direction-reopen-gpu-native-world-engine.md)）：**GPU 原生世界引擎为本体，评测降为第一应用**。核心文档三份：本文件 + `CURRENT.md`（本周）+ `README.md`（门面）；支撑层：`GLOSSARY.md`、`docs/adr/`、`docs/17-research/`、`docs/agents/`。旧方向的完整证据链与历史层积在 `docs/99-archive/`，只查不读。

---

## 一、这个项目是什么（3 句话）

1. **OmniWorld 是一个 GPU 原生世界引擎（C++20）：让世界数据——视频解码、AI 推理、观测渲染、世界状态更新——以最少实际拷贝、最少同步、最低延迟穿过 GPU 系统。**
2. 它的组织核心是一个 canonical 世界状态 + World Contract（identity / time / ownership / confidence）：感知模型、世界模型、仿真器、渲染器、智能体全部以薄 Adapter 接入同一个活的、可测量的世界。
3. 第一个应用是评测 harness：把模型插进来、跑闭环、出可复现指标——它验证引擎的正确性；评测工具的产品化是**证据触发的期权分支**（ADR-0008），不是默认义务。

**它不是**：游戏引擎、大模型本体、工业数字孪生（NVIDIA Mega 的领地，正面避开）。

**与历史方向的关系**：2026-09-21 曾以"评测优先"为身份（ADR-0007，生态目标函数）；2026-10-10 重开为引擎身份（ADR-0008，个人目标函数）。**终局画面未变（§三），变的是身份与顺序。**

## 二、想法族谱（你曾有的 40 项构想 → 六个去处，全部带回归条件）

> 2026-09-28 复盘补录：收敛没有删除任何想法，只是给它们排序、找支点。每个去处都有明确的回归规则。

| 去处 | 想法（原词） | 回归条件 |
|---|---|---|
| **① 支点 = 项目本体（活着）** | WorldState/Observation/Action/Transition 契约；"真实+合成世界统一"（grounding loop 正是合成半边，真实半边由视频适配器回归）；显存零拷贝纪律（化为 9 字段 buffer 契约 + per-stage 延迟计量——最初的技术论点，ADR-0008 起复位为本体）；回放数据输入 | 契约半边已兑现（Phase 0/1）；GPU 数据流半边自 Phase 2 起回归 |
| **② Adapter 归位（绕着支点长回来）** | NVDEC/NVENC/FFmpeg 视频管线；TensorRT（ONNX Runtime 先行顶位）；CV-CUDA 预处理；CARLA/Isaac 仿真接入；ROS 2/NITROS（≥6 个月）；Jolt 物理；Brain/LLM/VLA/WAM（自最小 Agent 契约生长） | "某 phase 的 benchmark 需要它"即回归 |
| **③ 技能项目（学习 + 作品集，不进主线承诺）** | Vulkan 1.3 离屏渲染器与效果栈：PBR、**Post-Process Pass（记忆中"特效"的真身）**、光追、3DGS/神经渲染；CUDA 深度优化（自定义 GEMM/WMMA） | 可选立项；首选候选：离屏 Vulkan 观察渲染器替换 CPU 光栅器（对 owner 的独特学习价值：首次从应用层下到 API 层） |
| **④ 被敲门砖轨道吸收（职业构想的现代形态）** | 十级专家阶梯；求职矩阵（NVIDIA/Tesla/Figure/宇树…）；GitHub 1k stars；CPU vs GPU 展示 demo（"80% vs 3%"） | 敲门砖轨道本身：每 Phase 对外工件 + 被 merge 的 PR——比攒 stars 更可执行 |
| **⑤ 明确让渡（有证据地不做）** | 数字孪生/工业（NVIDIA Mega 领地）；游戏引擎化/编辑器；分布式/移动端/Jetson；AD/ADAS 作为产品方向（评测 harness 仍可服务 AD 数据） | 证据变化才复议（Gate/ADR） |
| **⑥ 记忆澄清** | "特效/粒子/VFX"：仓库从未单独立项；来源=渲染效果栈（③）+ 收藏的粒子系统教程。不是记错，是从未命名——现在族谱里有了 | — |

另：`knowledge/` 的 20 个主题目录（cuda/vulkan/rendering/robotics/world-model…）原样保留——技能阶梯的学习地图，与主线不冲突。

## 三、终局画面（要做成什么样）

**原图即终局：视频/感知/渲染/仿真/Brain 全是 Adapter，绕着心脏（Canonical World State）长回来；世界数据全程以最少拷贝穿过 GPU 系统。**

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

- **1 年后的具体样子**：`omniworld run --adapter my_model.onnx --world scenes/mini_city.toml` → 真模型在环的闭环报告（帧率、身份稳定性、预测偏差、延迟分解）；一份被引用的 benchmark 笔记；一个被 merge 的 OSS PR；陌生人 clone 即跑。
- **5 年后（North Star）**：视频解码 → AI 推理 → 世界状态 → 渲染观测全程显存驻留零拷贝的统一 GPU 世界引擎（2026-08 蓝图原图）；其作者凭全程可复现的实测数字成为"渲染管线 + AI 系统"复合型工程师。
- **评测期权分支的远景**（外部信号出现才启动）：评测交互式世界模型时的默认接入路径——"MLPerf 之于测量"。

## 四、目标函数（2026-10-10 访谈确认，取代 09-24 双轨）

- **第一层 · 磨练技术**：登上 GPU 原生 AI 系统的技能阶梯——框架使用者 → 部署调优者 → 性能剖析者 → kernel 作者。起点（诚实记录）：OpenGL 应用层 + 渲染管线概念，尚无 GPU 代码经验；本项目的 Phase 顺序即课程表。
- **第二层 · 敲门砖（AI 相关岗）**：公开工件 = 作品集——可复现 benchmark 笔记、延迟分解、clone 即跑的引擎。差异化组合：渲染管线素养 + AI 系统实证。
- **第三层 · 创业期权**：不投入、不押注；唯一预注册的期权分支 = 评测工具产品化，由外部兴趣信号触发。
- **规则**：三层冲突时按序号裁决；作品集不夸大——"没有 benchmark 数据不宣称性能"同时是可信度资产。

## 五、范围四分

| 层 | 内容 |
|---|---|
| **CORE（自写）** | World Contract v0（第七节）、闭环 runtime（tick/版本/所有权裁决）、grounding 指标、确定性真值世界、离屏 CPU 光栅器（参考模式组件 + 未来 GPU 渲染的对照参考）、实体关联 |
| **ADAPTER（引用）** | ONNX Runtime（MIT）、RT-DETRv2-S 检测（Apache-2.0）、Depth-Anything-V2-Small 深度（Apache-2.0，仅 Small 可商用）、MCAP 录制（MIT）、回放数据、Jolt 物理备选（MIT）、ByteTrack-cpp 只读参考（MIT） |
| **EXPLORATION** | OpenUSD 单向导出、冲突策略族、不确定性传播、离屏 Vulkan 观察渲染器（=族谱③技能项目）、第三方开源世界模型接入（若 8GB 能跑） |
| **BACKLOG（不做）** | 付费大算力训练（个人预算红线）、ROS 2/VLA/LLM、分布式、游戏 demo、编辑器、商业化、OpenUSD 全兼容、Vulkan 完整渲染器、移动端 |

**训练边界（2026-10-10 修正）**：免费资源尺度内（本地 RTX 4060 8GB / Kaggle 免费 GPU）允许训练玩具级~小模型（如 Phase 1 的 ~3k 参数 MLP）；付费大算力不做。旧措辞"不训练任何模型"废止（与既成事实矛盾）。

**许可红线**：AGPL（Ultralytics YOLO 全系/FastSAM/boxmot）、CC-BY-NC（DepthAnything 非 Small）、无许可证（tinyrenderer）一律不进产品路径；vendored 代码放 `third_party/` 并保留原许可证。完整规则见归档 `third-party-policy.md`。

## 六、里程碑路线图（里程碑锚定，无日历；Gate 为唯一时间锚）

| Phase | 内容 | 出口（能力 Gate） | 对外工件 |
|---|---|---|---|
| **0+1 Grounding Loop v0 + 世界模型**（✅ 2026-09-28 完成） | 契约 + 闭环 + 指标；五世界模型经同一契约入环 | Gate 1 三判据 PASS（1106.7 Hz / 适配器零核心改动 / ECE 分离 0.9617） | 两份 regression 报告 |
| **2 AI 入环**（= 过程重开后的**阶段 C**） | ONNX Runtime（CUDA EP）真模型适配器（RT-DETRv2-S / Depth-Anything-V2-Small）；折入架构候选 #3（Camera→Observation、修 224 硬编码） | 真模型 = 单文件适配器、核心零改动；参考模式位级复现保持；性能模式延迟预算实测（4060 8GB） | 延迟分解 benchmark 笔记 |
| **3 真实数据入环** | NVDEC 硬解 + 真实视频回放作为 Observation 源 | 真实视频在环跑通；解码→推理路径延迟实测 | 视频管线工程笔记 |
| **4 世界模型/智能体应用** | 回放→预测闭环深化；Kaggle 免费资源训小模型（dynamics/depth 玩具级）；最小 Agent 契约 | 小模型训练可复现（seed 级）；agent 在环 | 训练+评测一体示例 |
| **（期权）评测工具产品化** | WorldMark/WBench 词汇对齐、协议公开发布 | 外部兴趣信号出现才启动 | — |

**2026-10-12 过程重开（ADR-0010）后的阶段与本表的对应**（2026-10-16 同步消歧义）：

| 阶段 | 内容 | 出口判据 | 对应本表 |
|---|---|---|---|
| **A 核心引擎重建** | 白纸重建最小闭环（6 切片） | Gate-1 对齐报告（✅ 2026-10-14 落库：(a)(b) PASS、(c) PARTIAL 如实标注） | 取代原 Phase 0+1 行 |
| **B 双模式框架** | ADR-0009 运行层（双模式 CLI/容差比较器/延迟计量）+ F1 per-producer ECE + F2 有界场景 | 参考模式逐字节不变（全量 ctest 绿）+ 性能模式容差自证（噪声 stub） | 新增（原散落在 Phase 2 行内） |
| **C AI 入环** | ONNX 真模型接入 | 真模型单文件接入 + 延迟预算报告 | 本表 Phase 2 行 |

各阶段目标·交付物·退出判据的详细划分表冻结于 [`docs/design/00-总体设计.md`](docs/design/00-总体设计.md)（L0）；阶段 L1 随各阶段开工前共创（`docs/design/stage-*/概要设计.md`）。

**Gate 体系（2026-10-10 重塑，ADR-0008；旧生态 Gate 2/3 废止）**：
- **能力 Gate**（技术判据，铁律）：每 Phase 出口判据如上表；不达标不进下一 Phase。
- **作品集 Gate**（敲门砖判据）：公开工件包成型——README 对外版（clone 即跑）+ 带环境元数据的 benchmark 笔记 + 一页工程笔记。
- **期权触发器**：外部兴趣信号（issue/引用/合作邀请）出现 → 重估评测工具产品化分支；不出现不构成失败。
- **时间止损**：连续 3 个月无任何公开工件产出 → 缩小范围重排。

## 七、World Contract（权威文本：docs/design/00-总体设计.md §2）

**契约已白纸重开共创并冻结（2026-10-12）**：9 条语义（实体属性 / 身份铸造 / tick 时间域 / 字段所有权 / Proposal 写路径 / 冲突 / 置信度 / 观察 / Tick 日志）+ 三项后视镜裁定（Action 未定预留；tick 即版本；来源由账本归属表达）。完整权威文本见 [`docs/design/00-总体设计.md`](docs/design/00-总体设计.md) §2（L0）；后视镜 delta 审查（4 重合 / 3 收紧 / 5 缺口处置）见 [ADR-0011](docs/adr/0011-contract-rear-mirror-delta-review.md)。

**继承的设计规则（不变）**：每个字段必须有真实消费者，没消费者就删；只加字段不改语义；语义变更 = 新 ADR。

**可复现性双模式（2026-10-10，[ADR-0009](docs/adr/0009-reproducibility-dual-mode.md)）**：参考模式（纯 CPU）逐字节复现，是契约回归的唯一 oracle；性能模式（GPU）容差复现 + 延迟预算，不承诺逐字节。字段语义零改动；性能模式数字必须标注模式开关与环境元数据。

**历史**：v0.1 契约原文（2026-09-28 冻结）仅存 git 历史（`pre-rebuild` 及更早的 PLAN.md）。

## 八、执行状态

> **本节不承载"现在做什么"。** 当前焦点与出口条件的唯一来源是 [`CURRENT.md`](CURRENT.md)。本节只保留**不随进度变化**的历史事实与重建对照基线，避免出现第二份进度快照。

- **重建对照基线**（阶段 A 出口的对齐目标）：Gate 1 三判据 1106.7 Hz / 适配器零核心改动 / ECE 分离 0.9617；五世界模型 divergence 五级分离。证据：[`benchmarks/regression/`](benchmarks/regression/)。
- **时间线**（只增不改）：2026-09-28 Phase 0/1 完成、契约 v0.1 冻结 → 2026-10-10 方向重开（ADR-0008；双模式 ADR-0009）→ 2026-10-12 过程重开（ADR-0010：旧实现作废、tag `pre-rebuild`，白纸共创重建）。
- **挂起的架构候选**（2026-10-09 评审，报告 [`docs/17-research/2026-10-09-architecture-review.html`](docs/17-research/2026-10-09-architecture-review.html)）：随旧实现一并归档，其设计洞见（如"裁决结果应携带它裁决的内容"）进入白纸重建的后视镜审查。

## 九、操作规则

1. 新想法 → `docs/17-research/research-queue.md`，不进 sprint；sprint 内只执行不扩写。
2. 没有 benchmark 数据不宣称性能；报告必须带完整环境元数据 **+ 运行模式标注**（参照 2026-09-18 baseline 格式）。
3. 研究只解当前 phase 出口所需的问题；产 research note 带来源。
4. **代码未跑通前不加新文档**；例外 = 用户明确要求、或代码先行验证后补记（如 `docs/agents/workflow-manual.md`，owner 2026-10-14 要求）。
5. **方向/契约/边界级决策 → `docs/adr/` 落 ADR，禁止以"补录"形式堆进本文件**（2026-10-10 新增：PLAN 层积是上次失焦的结构性根因）。
6. **设计先行**：任何代码切片动工前，必须有该切片经 owner 批准的详细设计（`docs/design/` L2）；每层设计以批准为冻结，变更走 ADR（2026-10-12，ADR-0010）。

## 十、证据与深度阅读（想查才查，不必读）

- 方向证据（旧方向，09-21）：`docs/17-research/2026-09-21-direction-decision.md`；方向重开（10-10）：[ADR-0008](docs/adr/0008-direction-reopen-gpu-native-world-engine.md)
- 复用与选型依据（18 仓库调研）：`docs/17-research/2026-09-24-github-reuse-map.md`
- 架构评审（5 候选，#3 已折入 Phase 2）：`docs/17-research/2026-10-09-architecture-review.html`
- 待研究问题与盯防清单：`docs/17-research/research-queue.md`
- 历史文档（旧规划/原蓝图报告/讨论记录/生命周期）：`docs/99-archive/`
