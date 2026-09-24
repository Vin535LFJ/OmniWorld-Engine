# OmniWorld 方向决策研究报告（2026-09-21）

> Trust label: [Research → Architecture Decision] — 本报告是 [ADR-0007](../01-architecture/decisions/ADR-0007-project-direction.md) 的证据基座；落地决策见 [vision.md](../00-overview/vision.md)、[master-roadmap.md](../02-roadmap/master-roadmap.md)。
> 决策日期：2026-09-21　|　采用文档落库：2026-09-23（e8dedf3）　|　本报告落库：2026-09-24（两次写入中断后补交，见文末诚实记录）
> 方法：六路独立研究（三路本地仓库勘察 + 三路联网核验），所有承重论断附主要来源 URL。

## 0. 执行摘要

- **最终方向：Strategy D——评测优先的 Grounding Loop Runtime**。OmniWorld 是开放的实时运行时：一个 canonical 世界状态 + identity/time/ownership/confidence 契约 + 闭环 grounding 指标，模型/仿真器/渲染器/智能体全部以薄 Adapter 接入；首个立足点是交互式世界模型评测社区正在重复手写的共享闭环 harness。
- **你的 World Runtime 直觉是对的，但"原样 B"不可行**：工业版已被 NVIDIA 以专有栈（Mega Omniverse Blueprint + `ovstage`）实现并占据；开放中立版本经核验**不存在**——但需要的是需求锚点，评测就是那个锚点。
- **白地已被实证**：2025–2026 每支评测团队都在重复实现 action 适配器 / 状态 schema / 仿真耦合（WorldMark 原话 "no shared protocol has been established"）；7 月起学术界论文（exact-state serving、world state registers）开始触碰同一层——窗口正在打开且不会一直开着。
- **边界**：不做工业数字孪生（NVIDIA 领地）、不训练世界模型、不做平台化布道直到 Gate 2 的外部信号出现。

## 1. 研究方法与证据等级

- 六路：① 仓库治理文档审计（README/docs/roadmap/lifecycle，全部读毕）；② 两份既有研究报告提取与核查（R1 中文全景报告、R2 deep-research-report.md）；③ 代码现实盘点（src/、CMakeLists、benchmarks、git 历史）；④ NVIDIA 栈官方核验（openusd.org / developer.nvidia.com / GitHub）；⑤ DeepMind / World Labs / 其他世界模型系统官方核验；⑥ 白地 prior-art 检索（ROS 2 文档、arXiv、GitHub 官方仓库）。
- 证据等级：A=官方文档/官方仓库/官方新闻；B=arXiv 论文；C=厂商技术博客；F=未能核实。本报告决策只依赖 A/B；C 仅用于产品事实；F 项明确标注，不作为论据。
- 环境限制（透明化）：检索环境中 WebFetch 失效率高，部分核验经由直接抓取官方页面完成；docs.omniverse.nvidia.com 对非浏览器客户端返回 403，涉及 Kit 内部细节之处标注未核实。

## 2. 对既有两份报告的核查与修正

| 来源论断 | 核验结果 | 证据 |
|---|---|---|
| R1：Cosmos 3 存在，版本覆盖 Super/Nano | ✅ 证实（并新增 Edge 4B，2026-07） | github.com/nvidia/cosmos README；arXiv 2606.02800；nvidianews 2026-05-31 |
| R1：动作空间 9D/10D/20D/29D/57D | ✅ 基本证实；"57D locomotion" 官方措辞为 57D egocentric motion | github.com/nvidia/cosmos README |
| R1：OpenMDW-1.1 许可（Linux Foundation） | ✅ 证实 | github.com/nvidia/cosmos；openmdw.ai/license/1-1/ |
| R1：USD Fabric "<5ms"、百万实体 GPU 常驻 | ⚠️ 未证实——官方文档被 WAF 阻挡；其现役后继 `ovstage`（GPU 常驻向量化场景底座、DLPack 零拷贝路径）属实但为 **NVIDIA 专有许可** | github.com/NVIDIA-Omniverse/ovstage |
| R1：Marble 生成耗时表（20–30s/2min/5min/1h） | ✅ 基本正确 | docs.worldlabs.ai |
| R1：Atlas 存在（+ "Shared Spatial Context"） | ✅ 证实（2026-09-01，multi modal autoregressive diffusion transformer，early access） | worldlabs.ai/blog/atlas |
| R2："RTFM"（若指 Recurrent Temporal Fine-tuning for World Models） | ❌ 名称错误；实为 **Real-Time Frame Model**（2025-10-16，单 H100 交互帧率的研究预览） | worldlabs.ai/blog/rtfm |
| R2：Genie 3 "from text prompts" | ⚠️ 时效性差异：2025-08 发布时以图像提示；现模型页为 "simple text descriptions"；限制是 720p/24fps、分钟级一致性 | deepmind.google/models/genie |
| R2："Isaac Sim 开源" | ✅ 证实（Apache-2.0） | github.com/isaac-sim/IsaacSim |
| R2："OpenUSD (Pixar/Apple)" | ❌ 治理归属错误：现为 AOUSD / Linux Foundation；Core Spec 1.0 已定稿（场景描述/组合/文件格式） | aousd.org |
| R2：AirSim 可用 | ❌ 已归档（后继为 Colosseum） | github.com/microsoft/AirSim |
| 两份报告共同宣告的 White Space | ✅**收敛证实但更窄**——见 §5 | 见 §5 |

## 3. 世界模型技术版图（2026-09 快照）

（精炼版；每类列最强代表与一般特征，来源见 §4/§11）

- **A 感知/世界理解**：VFM/VLM、3D 感知（D4RT，2026-01，统一 4D 重建/跟踪）、检测分割。特征：模型中心，输出是**对世界的断言**，不持有共享状态。
- **B 预测世界模型**：Dreamer 4（latent rollout 训练智能体，2025-09-29）、Cosmos-Predict 系。特征：内部 rollout，闭环评测协议缺失。
- **C 生成式世界模型**：Genie 3（无 API 状态）、RTFM（Real-Time Frame Model）、Decart Oasis 3 / Lucy 2.5、Runway GWM Worlds 2（720p/24fps + 音频，2026-09-03）等。特征：prompt/control→frames。
- **D 3D/空间世界模型**：World Labs Marble（持久 splat/mesh/碰撞体资产 + SPZ）、Atlas（早期访问）；学术拥挤（Matrix-Game 3.5 等）。
- **E 仿真**：Genesis / Isaac Sim / MuJoCo / 机器人仿真器族；仿真器在本项目定位为 Adapter。
- **F 渲染**：RTX（NVIDIA 专有）/ SparkJS（开源，World Labs）；渲染器在本项目是 Observation 产出者。
- **G Agent / Embodied**：SIMA 2（屏幕像素+键鼠，无特权状态）、Gemini Robotics 系、1X World Model Lab；Agent 在本项目只有一个最小 Action 契约。

## 4. 关键系统核验（决定本项止边界的六组事实）

### 4.1 OpenUSD（AOUSD）
- 官方定位是场景描述 + 组合（composition arcs）+ 交换：单层组合引用、非破坏性覆盖（openusd.org/release/intro.html）。
- USD 26.08 附带 **OpenExec**（值驱动的 DAG 计算图），其官方 "What OpenExec Is Not" 明确"非事件驱动、不改场景描述"（openusd.org/release/intro_to_openexec.html）。
- AOUSD Core Specification 1.0 已定稿，内容只覆盖语法/数据类型/组合算法/文件格式（aousd.org/blog Core Spec 1.0）——**运行时同步、状态所有权、模型服务、不确定性不在标准范围**。
- 结论：OpenUSD 是"数据模型 + 文件语义"，不是运行时；它对 OmniWorld 的价值是（Exploration 级别的）单向导出目标，不是竞争对象。

### 4.2 NVIDIA Omniverse / Mega / ovstage —— 工业版 world runtime 已存在且专有
- Omniverse 在 2026 年被重新定位为 "Physical AI 库栈"，官方自述是"连接物理世界数据与 Physical AI 的操作系统"（nvidianews 2025-03；developer.nvidia.com/omniverse）。
- **Mega Omniverse Blueprint** 已实现 perception→World State→simulation→agent 闭环：World Simulator（live OpenUSD stage）维护仿真状态并同步各组件；机器人大脑以容器接入（ROS 2 topic / gRPC / VDA5050）（developer.nvidia.com/blog/simulating-robots-in-industrial-facility-digital-twins/）。
- GPU 常驻场景底座 `ovstage` 为源码可见的 **NVIDIA 专有许可**（github.com/NVIDIA-Omniverse/ovstage）；开放对位物 omniverse-sil-bridge（Apache-2.0）通过 OVSimDataAPI/gRPC 暴露仿真数据（github.com/NVIDIA-Omniverse/omniverse-sil-bridge）。
- 结论：**这条赛道没有 OmniWorld 的位置**——既缺乏中立性，也无法在硬件/许可上与之竞争。

### 4.3 NVIDIA Cosmos 3 —— 开放权重的模型，不是 runtime
- 2026-05-31 发布（GTC 台北），全模态 MoT 世界模型族：Super 64B / Nano 16B / Edge 4B；附 inference benchmarks 与 Apache-2.0 的 cosmos-evaluator（自动化"世界生成/世界推理"结果评分），OpenMDW-1.1 许可（arxiv.org/abs/2606.02800；github.com/NVIDIA/Cosmos、cosmos-evaluator、cosmos-framework）。
- 硬件现实（与本项目直接相关）：Nano 16B 目标为 RTX PRO 6000 / H100 / B200 级，Edge 4B 目标 Jetson Thor / RTX PRO 6000——**本机 RTX 4060 8 GB 上 Cosmos 不能跑**（README 硬件表）。
- Cosmos 仓库内提供的 evaluation/ 复现配方（Physics-IQ、PAI-Bench、AMT-S）是**开环视频评分**工作流，不是闭环 runtime（见 §4.7）。

### 4.4 Google DeepMind Genie 3 —— 无 API、无状态暴露
- 官方定位："a general-purpose world model … explored in real-time"（2025-08-05 博文；deepmind.google/models/genie）：720p、20–24fps 导航、一致性"几分钟"。
- 可用性：研究预览；公网入口是消费级实验 **Project Genie**（Google Labs，2026-01-29，2026-05-21 扩展至 Google AI Ultra $200/月，含 Street View grounding；Google 称 Waymo 用于道路环境研究）。**无 API、无状态查询、无导出**。
- 相关工作：Dreamer 4（arxiv 2509.24527）；SIMA 2（纯屏幕像素+键鼠操作，无特权状态访问，2025-11-13）；D4RT（2026-01-22）。结论：DeepMind 的一切对外皆模型中心，接地要靠屏幕像素。

### 4.5 World Labs —— 思想上最接近，产品上是内容平台
- Marble（GA 2025-11-12）：由文本/图像/视频/360°/Chisel  coarse 3D 积木生成**持久 3D 世界**（3D Gaussian splat SPZ + mesh + collider mesh + 全景图），生成耗时官方表与本报告 §2 一致。
- **World API**（2026-01-21）：REST，`worlds:generate` 等端点；返回**持久可寻址的世界资源**（world_id，get/list/export/delete），资产含 scale/ground-plane 的 semantics_metadata、cargo PLY/GLB 导出与 Unreal/Unity/Blender 插件；计费制（1,500 积分 ≈ $1.20）——是"世界即资产"而非"世界即活状态"。
- RTFM = Real-Time Frame Model（2025-10-16，单 H100 交互帧率的 generative world model 研究预览，状态隐式存于 KV cache）；Atlas（2026-09-01，omni model，early access）；收购 SceniX（2026-07，专有 R2S2R 仿真引擎）。
- World Labs 是唯一明确理论化该层的公司（"taxonomy of world models" 2026-06-03；"3D as code" 2026-03-03——批评 frame-by-frame 黑盒世界是"状态外部化"的对立面），但只要其仿真资产/引擎闭源，其生态就不是中立 runtime。
- 结论：在这个谱系上最思想同源，也因此是**最需要差异化定位**的对象；其资产导出是可选 Adapter。

### 4.6 其他代表性玩家（2026-09 现状一句话）
- Decart：已转向"低延迟 AI 基础设施"（Lucy 2.5 30FPS 直播变换、Oasis 3 面向 Physical AI；2026-05 获 $300M）——模型中心，无状态 API。
- Odyssey：$1.45B 估值 Amazon 支持（2026-06），品类是 world model，交付物是模型/视频，无开放 runtime。
- Runway：GWM Worlds 2（2026-09-03，720p/24fps 交互视频+48kHz 音频）；Robotics 产品线（policy 模型/离线评估/数据增强）。
- Microsoft：WHAM/Muse 是研究叙事，无产品/运行时。
- 1X（世界模型 + World Model Lab）、Physical Intelligence（VLA）——模型中心。
- 学术：交互式世界模型拥挤（Matrix-Game 3.5、ReWorld、Sekai2、GeniWorld、H3-World 等），**开始出现基础设施型论文**："Streaming Multi-Agent ARD with World State Registers"、"Stateful Worlds, Stateless Elasticity: Exact-State Serving for Interactive World Models"（2026-07）。
- 总结：**所有人都在发模型/帧/资产；没人发可分享的活世界状态运行时层**。

### 4.7 评测生态 —— 需求实证所在
开放且可插拔的交互式评测在 2025–2026 爆发，且全部是"论文 + 脚本"型 harness：
- WBench（arxiv 2605.25874，美团 LongCat+复旦；5 维 22 指标、leaderboard）；WorldMark（arxiv 2604.21686；共享 WASD 控制词汇经 per-model adapter 翻译，**原话 "no shared protocol has been established"**）；WorldRoamBench（2606.31672，长时程）；iWorld-Bench（2605.03941，统一动作框架）；WorldArena（2602.08971，world-arena.ai，CVPR 2026 challenge）；TriWorldBench（2026-07-28，同一世界三视角一致性 vs 真值锚点）；KineBench（2607.19876，ECCV 2026，把生成视频中的 6D 动作抓回 ManiSkill3 物理里回放评分）；AutoWorldModel-Bench（2608.11216，8 个游戏导出 ground-truth 实体状态张量的统一协议）；stable-worldmodel（2605.21800，galilai-group，自带 BYO-model 的采集/训练/MPC 评测）；WorldFoundry（github.com/OpenEnvision/WorldFoundry，2026-07-11 开源，首日 317 星，"video generation / 3D / 4D / embodied action / interactive worlds 的共享 runner+推理+benchmark 栈"）；EasyWAM（OpenMOSS，WAM 统一 train/eval 底座）。
- 它们的共同点：**每一家都自行重写了 action adapter、状态 schema、sim 耦合**；没有任何一家提供"canonical 世界状态 + 面向异构参与者的运行时契约"。
- 这就是 OmniWorld 立足的需求锚点（评测侧，不是工业侧）。

### 4.8 ROS 2 与机器人中间件（作为对照的"已有解法"）
docs.ros.org 官方机制：坐标变换 tf2（命名 frame 树 + 时间戳查询 + 静态 transform）；ROS Time/`/clock`（use_sim_time 时仿真器是时钟源）；message_filters 精确/近似时间对齐；DDS 分布式发现。
不提供：中间件级实体身份（只有 UUID 消息约定）、世界状态存储、所有权仲裁（DDS ownership-strength 在 ROS 2 API 中未暴露）、渲染、GPU 常驻状态、模型契约。结论：ROS 2 是运输层，不做本项目的状态契约层；因此本项目 6 个月内不做 ROS 2 桥（Backlog）。

## 5. White Space 判定

| Existing System | Solves | Does Not Solve | Overlap with OmniWorld | Possible Gap |
|---|---|---|---|---|
| OpenUSD (AOUSD) | 场景描述/组合/文件 | 运行时同步、所有权、模型服务、不确定性 | 仅"数据模型"层 | 可用作单向导出对象 |
| Omniverse Mega + ovstage | 工业 fleets 的闭环运行时 | 中立性、非 NVIDIA 模型接入面、开放许可 | 若 OmniWorld 做工业孪生则正面冲突 | **留出的空档：开放/中立/研究侧** |
| Cosmos 3（NVIDIA） | 开放权重 omni 模型 + evaluator | 运行时/状态层；H100 级硬件门槛；开环评分 | 评测对象（经回放数据） | 闭环 grounding 测量与消费级硬件运行 |
| Genie 3 / Project Genie | 交互世界生成 | 一切外部集成 | 无 | 潜在未来适配对象 |
| World Labs（Marble/World API/SceniX） | 持久可寻址世界资产 + 生成 | 活状态、物理步进；仿真引擎闭源 | 思想最接近（状态外部化叙事） | OmniWorld 的开放实时 runtime 正是其未做的部分 |
| WBench/WorldMark/WorldArena/KineBench 等 | 交互评测协议与指标 | 共享运行时；每队重复造 adapter/schema | 目标用户重叠 | **本体**——它们缺的 runtime |
| WorldFoundry | 开源共享 train/eval 栈 | live 状态、实时帧预算、grounding 契约 | 最近的开源邻居 | OmniWorld 状态中心 vs 其模型中心 batch——需 Phase-1 差异对比 ADR 确认 |
| ROS 2 | 传输/坐标/时钟/QoS | 实体身份/世界状态/所有权/渲染/模型语义 | 机器人部分问题域 | 状态契约层（ROS 2 定位为 Adapter） |
| Genesis / Isaac Sim | 仿真 | 跨模型 grounding | Adapter 角色 | 接入对象，非竞争者 |

**判定：白地存在，但比 R2 报告声称的更窄、更清楚**——"开放、中立、面向研究/评测的实时 grounding 闭环 runtime"无人提供；广义"连接一切平台"的白地不存在（NVIDIA/World Labs/评测项目分别从两端压缩）。**负面证据的义务性注意**：直接"签名式"需求未找到（检索 GitHub/arXiv/vendor 论坛）；需求被推断——来自 fragmentation 承认 + 集成劳动的可见重复（WorldFoundry/EasyWAM/cosmos-framework 的大量代码用于集成别人的模型）；因此 Gate 2/3 设定外部信号为硬退出条件，不是乐观假设。

## 6. 十项质疑的逐条回答

1. **OmniWorld 是否需要存在？** 需要，但只以 D 的尺度存在：作为评测社区正在重复手写的那个闭环。B 全景（平台化 substrate）的需求没有被证实，且被 NVIDIA/World Labs 两端压缩。
2. **World Runtime 层是否已被 OpenUSD/Omniverse/Cosmos 覆盖？** 语义/工业闭环被覆盖（AOUSD spec 只到文件层；Mega 是闭源+NVIDIA 生态）；开放中立研究侧的空缺确凿。
3. **World State 连接理解与渲染是否只是表面概念？** 非表面：NVIDIA 真的为它建了专有底座（ovstage）、World Labs 公开将其理论化、2026-07 起学术出现 serving/state 论文。风险不是"概念不成立"，而是"无锚点的抽象"。评测就是锚点。
4. **是否适合个人/小团队长期开源？** 完整平台不适合；D 适合（单一闭环、模型皆适配器、全本地可复现、Gate 止损）。
5. **是否应做 World Runtime？** 应该，以 D 的形态进入。
6. **是否继续保持 GPU Runtime？** 保留为**地基**（帧预算、零拷贝、延迟计量是你的差异化技能），不再是身份认同。
7. **是否直接做 World Model？** 否（算力/数据不可行；用户明确约束）。
8. **更窄且更有壁垒的方向？** 即 D：壁垒=World Contract 语义（identity/ownership/time/confidence）+ 闭环 coherence 指标 + 中立参考实现的先发积累。
9. **是否把已有技术拼起来？** 有这一风险；缓解判据固定为"说不出只有 OmniWorld 能测的指标，就不进 Core"。
10. **是否有可验证的技术差异化？** 有——Gate 1 的三条证伪判据（30Hz / adapter-only 接入 / 指标判别力），任一失败即 thesis 失败。

## 7. 战略对比（A/B/C/D）

| Dimension | A GPU Runtime | B World Substrate（原样） | C World Model | **D 评测优先 Grounding Runtime（选）** |
|---|---|---|---|---|
| Technical feasibility | 高 | 中 | 低 | 高（复用 A 的地基） |
| Research difficulty | 低 | 中-高 | 极高 | 中 |
| Engineering difficulty | 中 | 极高 | 极高 | 中 |
| Solo/small-team feasibility | 高 | 低 | 不可行 | 高 |
| Differentiation | 低（无存在理由） | 高但不可及 | 不适用 | 中-高（契约+闭环+中立） |
| Open-source value | 低 | 高但难启动 | 高但不可行 | 高（已被实证的需求锚点） |
| Dependence on foundation models | 低 | 高 | 极高 | 低-中（模型皆 Adapter） |
| GPU Runtime relevance | 本体 | 底层 | 弱 | 底层（差异化技能被用上） |
| World Model relevance | 弱 | 高 | 本体 | 高（作为被测对象进入） |
| Rendering relevance | 中 | 高 | 弱 | 中（Observation 生成即可） |
| Simulation relevance | 弱 | 高 | 弱 | 高（真值世界是核心组件） |
| Agent relevance | 弱 | 高 | 中 | 中（Phase 2 扩展的最小接口） |
| Long-term potential | 低 | 高（若成） | 极高（若成） | 中-高（若被用可长为 B；有 Gate 止损） |
| Competitive pressure | 高（DALI/NIM/工具链） | 极高（Mega 正面） | 极高 | 中（WorldFoundry 最近邻，需盯防） |
| Ability to start small | 高 | 低 | 不可行 | 高 |

要点：B 的前景与 D 不矛盾；D 是 B 的需求验证子集，Gate 2/3 的设计意图就是"在证据出现前不为 B 的平台化买单"。

## 8. Canonical World State 的可行性（对 §第二节假设的逐项裁决）

| 难点 | 裁决 | 依据 |
|---|---|---|
| Entity identity | **v0 半解**：runtime 铸 ID + 最近变换匹配 adjudication | 跨模型身份无标准（Hydra/ConceptGraphs/Khronos 皆单系统）；v0 只需可用，跨画面重识别是研究队列 |
| Coordinate frames | **v0 解**：单一世界系，view frame 只存在于 Observation | 复杂度推迟，不忽略 |
| Time | **v0 解**：单一 tick 域 + 每 stage 延迟计量 | ROS 2 有 sim-time 实践但无跨模型一致性 |
| State ownership | **v0 解**：真值世界独有 kinematics 字段，模型只写 Proposal | DDS ownership-strength 思想（ROS 2 未暴露）；冲突数据驱动 v0.1 |
| Uncertainty | **v0 最小解**：置信度浮点 + 校准判别实验（Gate 1c） | 无跨模型不确定性标准；但评测的可分辨性测试依赖它 |
| Temporal consistency | **v0 解**：tick/版本号 + staleness 可测 | 与 §4.7 评测协议指标兼容 |
| Conflict resolution | **记录不裁决**（v0 truth-wins）；策略族延至 Phase 3 | 由 MVP 冲突分布数据驱动 |
| Partial observation | **v0 单视角**；遮挡语义进入 v0.1 研究 | — |
| Multiple representations | 明确 defer | World Labs "3D as code" 方向相关；不在 v0 |

**结论**：通用最小契约存在（v0 的字段集）；全能"World State Schema"不存在；v0 用"usage-driven / additive-only"规则防止 schema 空想化。

## 9. 风险登记（R1–R8，触发条件与缓解）

1. **Scope explosion**：单月内引入第 4 个适配器/新方向想法进 sprint → 每月强制 review not-now；sprint 内只执行不扩写。
2. **重造 OpenUSD/Omniverse**：发现自己做工业 fleets/sim-first 工作流 → 永久边界：研究/评测侧；OpenUSD 仅单向导出。
3. **技术大杂烩**：Core 中出现无独立指标贡献的功能 → 任一功能需说出"只有 OmniWorld 能测的指标"否则不进 Core；架构评审强制引用此句。
4. **WorldState 过度理想化**：v0 加入未被 Week 2/3 代码消费的字段 → 字段必须有消费者，未消费即删；v0.1 冻结前不许加。
5. **无真实用户**：Gate 2（6 个月）无任何外部真实信号 → 降级为个人研究仪器并停止平台化投入。
6. **基础模型变化依赖**：基准主线必须全本地可复现；任何付费 API 都只做可选适配器演示，不进主线。
7. **单人壁垒不足**：18 个月内未形成（契约先发+指标体系+外部使用）→ 承认失败，止损退出，转向个人技术资产叙述。
8. **永远规划不做 MVP**：新 idea 一律走 Research Queue→Backlog；保真已提交（bb05d26）；本报告后规划冻结（见 master-roadmap 操作规则）。

## 10. 最终决策

- **Thesis**：交互式世界模型正被要求成为"世界模拟器"与"训练智能体的环境"，但没有开放的系统让它们与真值仿真器、渲染器和智能体共享一个活的、带契约的世界状态——这个能力要么锁在 NVIDIA 专有栈里，要么在每个评测项目里被重手写。OmniWorld 是这个闭环的开放 runtime：canonical 世界状态 + identity/time/ownership/confidence 契约 + 薄适配器 + 可测量（而非假设）的 grounding。
- **North Star（5 年，由 Gate 1/2/3 判定）**：当有人构建或评测交互式世界模型，"接入 OmniWorld 跑闭环 grounding 指标"是默认路径——如同 MLPerf 之于测量。OmniWorld 是让异构 AI 系统保持在同一演化世界中的开放 runtime。
- **Core Problem**：Cross-model grounding——在一共享、随时间演化的世界状态上，保持实体身份/坐标/时间/所有权/不确定性连贯，并在实时帧预算内可测量。
- **Scope**：Core / Adapter / Exploration / Backlog 见 vision.md。
- **MVP**：Grounding Loop v0（"one world, two writers, measurable coherence"，RTX 4060 8GB 全本地），三条证伪判据即 Gate 1。
- **Roadmap 与 30 天计划**：master-roadmap.md（Phase 0–4 + Gate 1/2/3 + 不做清单）与 current-sprint.md（Week 1–4，逐周可验证出口）。
- **继承结构**：保真提交（runtime_probe 全量入库）、归档（2026-09-planning-era，git mv 可回溯）、治理文件重分类完成。

## 11. 来源清单（可复核；外链均为官方/论文页面，检索于 2026-09-20/21）

- OpenUSD/AOUSD：openusd.org/release/intro.html；openusd.org/release/intro_to_openexec.html；aousd.org；aousd.org Core Spec 1.0 公告
- NVIDIA：github.com/nvidia/cosmos（Cosmos 3 README/硬件表/评测配方）；arxiv.org/abs/2606.02800；nvidianews 2026-05-31 / 2026-07-15（Japan）；developer.nvidia.com/omniverse；developer.nvidia.com/usd；developer.nvidia.com/blog/simulating-robots-in-industrial-facility-digital-twins/；build.nvidia.com/nvidia/mega-multi-robot-fleets-for-industrial-automation；github.com/NVIDIA-Omniverse（ovstage、omniverse-sil-bridge、PhysX、kit-app-template）；github.com/isaac-sim/IsaacSim（Apache-2.0）；github.com/NVIDIA/skills；github.com/NVIDIA/cosmos-evaluator（Apache-2.0）；github.com/NVIDIA/cosmos-framework；github.com/NVIDIA/Cosmos issues #21/#89（被关闭的 Omniverse 桥接请求）
- DeepMind：deepmind.google/blog/genie-3-a-new-frontier-for-world-models/；deepmind.google/models/genie/；labs.google/projectgenie；deepmind.google/blog/sima-2-an-agent-that-plays-reasons-and-learns-with-you-in-virtual-3d-worlds/；deepmind.google/blog/d4rt-teaching-ai-to-see-the-world-in-four-dimensions/；arxiv.org/abs/2509.24527
- World Labs：worldlabs.ai/blog/marble-world-model；docs.worldlabs.ai（API/Marble/Pricing/Chisel）；worldlabs.ai/blog/announcing-the-world-api；worldlabs.ai/blog/rtfm；worldlabs.ai/blog/atlas；worldlabs.ai/blog/taxonomy-of-world-models；worldlabs.ai/blog/3d-as-code；worldlabs.ai/blog/scenix；worldlabs.ai/blog/real-to-sim-to-real；worldlabs.ai/blog/spark-2.0；worldlabs.ai/blog/funding-2026
- 评测生态：arxiv 2605.25874（WBench）；github.com/meituan-longcat/WBench；arxiv 2604.21686 + github.com/AlayaLab/WorldMark；arxiv 2606.31672（WorldRoamBench）；arxiv 2605.03941 + github.com/EmbodiedCity/iWorld-Bench；arxiv 2602.08971 + github.com/tsinghua-fib-lab/WorldArena + world-arena.ai；github.com/TriWorldBench/TriWorldBench；arxiv 2607.19876 + github.com/minecraft-zzz/KineBench；arxiv 2608.11216（AutoWorldModel-Bench）；arxiv 2605.21800 + github.com/galilai-group/stable-worldmodel；github.com/OpenEnvision/WorldFoundry；github.com/OpenMOSS/EasyWAM
- 对比参照：ROS 2 文档（tf2、clock_and_time、message_filters、QoS、ros_on_dds）；arxiv 2201.13360（Hydra）；arxiv 2309.16650（ConceptGraphs）；arxiv 2402.13817（Khronos）；arxiv 2211.15654（OpenScene）；arxiv 2403.09227（BEHAVIOR-1K）；arxiv 2606.13840（MAEAD 综述"verifiable shared-state maintenance"）；arxiv 2510.10325（KG-MAS）；arxiv 2609.16697（"From Plausible to Controllable to Actionable"，2026-09-15）；arxiv 2606.01164（Towards Interactive Video World Modeling 综述）；github.com/Genesis-Embodied-AI/Genesis；physics-IQ（arxiv 2501.09038；google-deepmind/physics-iq-benchmark）；Physics-IQ Verified（arxiv 2606.18943）；VMBench（2503.10076）；WorldModelBench（2502.20694）；PAI-Bench（2512.01989）；PhyGenBench（2410.05363）；T2VPhysBench（2505.00337）；VideoPhy（2411.02385）
- 其他玩家：decart.ai/blog；runwayml.com（GWM Worlds 2 / Robotics）；venturebeat.com（1X world model，2026-01）；TechCrunch（Odyssey，2026-06-17）；microsoft WHAM/Muse（Nature/Xbox Wire，2025-02-19）；phy…（其它见对应节）
- 本仓库内部证据：benchmarks/regression/2026-09-18-runtime-probe-baseline.md；两份根目录研究报告（已核查修正，见 §2）；治理与生命周期文档。

## 诚实记录（本节不可删）

1. 2026-09-21 方向被采纳，但本报告与 vision.md 的第一次写入在中断中丢失；曾有一轮总结宣称存在提交 `c310f6d`——该提交**从未发生**。实际落库链为 `bb05d26`（2026-09-22，保真）→ `e8dedf3`（2026-09-23，方向采用含大部分治理文档）→ `9c31d01`（2026-09-24，vision.md + projects 索引修正）→ 本报告补交（2026-09-24，前两次写入报告"成功"但文件在提交前消失，第三次经由探针文件名写入后原子改名落盘）。凡是仓库外认知与 git 不一致之处，以 git 为准。
2. 本文档文件名使用"决策日期 2026-09-21"；各文档的落库日期以 git 历史为准（2026-09-22 至 2026-09-24）。
3. 依据第一章环境限制：少数标注"未证实"的论断（如 USD Fabric 内部数值）未被用于任何决策。
