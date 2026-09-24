# GitHub 复用地图与贡献目标（2026-09-24）

> Trust label: [Research Note] — 补强 [2026-09-21-direction-decision.md](2026-09-21-direction-decision.md) 的 §4.7/§5（无冲突；本笔记新发现 5 个评审系统并给出全部组件的选型与许可核验）。
> 方法：三路并行 GitHub 检索（评测 harness 生态 / MVP 组件候选 / "canonical world state"形似 prior art），GitHub API 实时核验 stars/license/活跃度。

## 0. 总结论

1. **白地再确认**：带 identity/time/ownership/confidence 契约的 canonical 活世界状态运行时，GitHub 上不存在。检索词 `world state server` / `shared world state` / `world model serving runtime` / `世界模型 共享 状态` 等只返回课程作业与玩具。"world model serving" 这个词目前占据者是 vLLM 式推理管道（如 WMFactory，55★），不是状态层。
2. **两派各缺一半**：闭环派（stable-worldmodel、World-In-World、KineBench——模型和真环境真的循环交互）没有共享状态对象；状态派（AutoWorldModel-Bench 的 per-entity 状态张量、CARLA 的 tick 快照）不做异构模型运行时闭环。OmniWorld = 融合这两半。
3. **MVP 的全部组件都有成熟开源件可引用**，许可已逐一核验；真正的危险点是许可污染（见 §3）。
4. **敲门砖通道成立**：评测社区的 MIT/Apache 活跃仓库（WorldMark、stable-worldmodel、WBench、cosmos-evaluator、WorldFoundry）接纳适配器/用例/指标类贡献——被 merge 的 PR 是可核验的外部信号（Gate 2 双轨之一）。

## 1. 评测生态仓库总表（闭环程度 1=纯离线打分 … 5=真环境实时闭环）

| 仓库 | Stars | License | 活跃度(最近 push) | 可复用件 | 闭环 |
|---|---|---|---|---|---|
| meituan-longcat/WBench | 240 | MIT | 2026-09-24 | 用例格式 + 22 指标套件 + 提交即入榜流程（289 全量/158 导航用例已被第三方提交，事实互操作标准） | 2 |
| AlayaLab/WorldMark | 45 | MIT | 2026-08-05 | **最强的适配器契约蓝本**：统一 W S A D L R 动作词汇 + per-model 翻译适配器 + "file:line 证据"验收门；9 个确定性指标 | 1.5 |
| tsinghua-fib-lab/WorldArena | 268 | 无许可证文件 | 2026-07-22 | 16 视频指标 + "功能效用"三轨（合成数据/策略评估/动作规划）；HF 排行榜 | 3 |
| EmbodiedCity/iWorld-Bench | 21 | Apache-2.0 | 2026-07-15 | 指标 API 封装范式 + 10 个参考推理适配器目录 | 1 |
| minecraft-zzz/KineBench | 6 | MIT | 2026-07-29 | **WM→仿真耦合最近邻**：视频→MoGe 深度+YOLO+FoundationPose→PyRoki IK→ManiSkill 回放打分；HTTP 生成服务契约可抄 | 4 |
| TriWorldBench/TriWorldBench | 233 | 无许可证文件 | 2026-09-23 | 三视角同步协议 + 轨迹派生的状态标注格式 | 2 |
| WorldRoamBench | — | 无公开仓库（仅站点 worldroam.amap.com） | — | 仅协议描述 | 2 |
| electronicarts/AutoWorldModelBench | 3 | BSD-3-Clause | 2026-08-13 | **最好的状态/实体协议参考**：registry[N,34] + states[T,N,23] + actions[T,7] + masks + meta.json；4 个入门 WM 类 | 4 |
| galilai-group/stable-worldmodel | 2,215 | MIT | 2026-09-22 | 采集→训练→MPC 评测统一平台；LeRobot 适配 | **5** |
| OpenEnvision/WorldFoundry | 318 | Apache-2.0 | 2026-09-10 (v0.2.0) | runner 注册表 + benchmark 产物布局契约 + scorecards；**预告 9 月底大更新直打 interactive worlds 评测——盯防** | 2–3 |
| OpenMOSS/EasyWAM | 328 | 自定义(未识别) | 2026-09-23 | WAM 统一 train/eval 接口、可续评测 | 2 |
| NVIDIA/cosmos-evaluator | 52 | Apache-2.0 | 2026-07-08 | checker 微服务（幻觉/对象对应/属性检查）+ Arbitrator 编排骨架——指标管线架构参考 | 1 |
| NVIDIA/cosmos-framework | 540 | 自定义(未识别) | 2026-09-23 | Cosmos 训练/服务栈；推理侧适配参考 | n/a |
| World-In-World/world-in-world（ICLR'26 Oral） | 208 | 见仓库 | 2026-04 | **真正的闭环评测**：WM 经统一动作 API 提议动作→LIBERO/导航 sim 执行→计分 | 4.5–5 |
| AMAP-ML/Omni-WorldBench | 108 | 无许可证 | 2026-03 | "动作对状态轨迹的因果影响"指标思想（Omni-Metrics） | 2 |
| CSU-JPG/MIND | 81 | 见仓库 | 2026-07 | "重访/返回场景"检验记忆一致性的协议 | 3 |
| TeleHuman/WNM-3D | 115 | 见仓库 | 2026-09 | WM 条件化跑在 VLN 仿真器内的闭环 | 4 |
| TuojingAI/CounterScene | 89 | 见仓库 | 2026-08 | WM+因果生成对抗场景，仿真耦合测试 | 3 |

## 2. MVP 组件决策表（选型 / 许可 / 理由 / 工作量）

| 组件 | 选型 | License | 理由 | 工作量 |
|---|---|---|---|---|
| 推理底座 | ONNX Runtime（microsoft/onnxruntime，NuGet `Microsoft.ML.OnnxRuntime.Gpu.Windows`；CMake 模式抄 onnxruntime-inference-examples） | MIT | 官方维护、CUDA 12.x 即装即用；DirectML EP 为备选 | S |
| 感知模型 A（检测） | RT-DETRv2-S（lyuwenyu/RT-DETR，官方 export_onnx.py + 部署示例） | Apache-2.0 | 非 AGPL 的最干净检测器；20M 参数，T4 上 65FPS，4060 上 100Hz+ | S/M |
| 感知模型 B（深度） | Depth-Anything-V2-**Small**（DepthAnything/Depth-Anything-V2；自持 .pth 自导出 ONNX） | Apache-2.0（**仅 Small**） | 与检测器架构互补（BEV 类 vs ViT 深度）；导出一次即可 | M |
| 感知模型 B 备选 | PP-YOLOE-small（PaddleDetection）/ MobileSAM（ChaoningZhang/MobileSAM） | Apache-2.0 | 架构多样性备选；MobileSAM 有官方 ONNX 脚本 | M |
| 真值世界 | **自写 ~2 页确定性运动学积分器**（v0 主选）；Jolt（jrouwe/JoltPhysics）为 Phase 3 外部物理备选；Box2D v3 单线程为 2D 备选 | MIT/MIT | 决定性是我们 thesis 的关键性质：自写=完全可控可审计，移除 10 万行依赖；Jolt 有书面确定性契约（"同序调用 API 则确定"） | S / M |
| 观察渲染 | **自写 ~200 行 CPU 光栅器**（224×224 平面着色、Z-buffer；设计上参考 zauonlok/renderer） | MIT(参考) | 此分辨率下 CPU 光栅微秒级、位级确定、零依赖 | S |
| 关联参考 | ByteTrack-cpp（Vertical-Beach/ByteTrack-cpp，只读参考；关联逻辑在 Core 内自实现 ~200 行） | MIT | IoU+匈牙利+恒速 Kalman 骨架清晰；身份裁决属 Core 语义，不外包 | S |
| 录制/回放 | MCAP C++ writer（foxglove/mcap） | MIT | 两文件即成；白送 Foxglove Studio 可视化（Phase 1 回放用） | S |
| 实体注册表模式 | 参考 EnTT（代际 ID）与 flecs（帧末命令缓冲）；v0 自实现小 registry（~300 行 slot map 模式，参考 orlp/slotmap, Zlib） | MIT/MIT/Zlib | 只取"代际 ID + 确定性迭代"两个模式，不引入 ECS 全家桶 | S |

## 3. 许可红线（已核验，务必避开）

| 项目 | 许可 | 风险 |
|---|---|---|
| ultralytics/ultralytics（YOLO11/v8 全系及其权重与导出脚本） | **AGPL-3.0** | 网络触发 copyleft，商业化即传染——**产品路径全禁** |
| CASIA-LMC-Lab/FastSAM | **AGPL-3.0** | 同上（基于 Ultralytics） |
| mikel-brostrom/boxmot | **AGPL-3.0** | 同上（tracker 槽） |
| Depth-Anything-V2 Base/Large/Giant | **CC-BY-NC-4.0** | 非商用；**只有 Small 是 Apache-2.0** |
| ssloy/tinyrenderer | **无许可证** | 24k 星≠可用；All-rights-reserved，只读参考、不得抄码 |
| onnx/models 仓库 | 仓库 Apache-2.0 但**逐模型许可各异** | 仅作烟雾测试用途 |

治理落地：见 [`../03-engineering-lifecycle/third-party-policy.md`](../03-engineering-lifecycle/third-party-policy.md)。

## 4. 值得抄的设计模式（不引代码，只学形态）

1. **CARLA**（carla-simulator/carla，MIT）：tick 编号的不可变 WorldSnapshot + actor 注册表——多消费者零锁一致读；每个产物自带 tick。最便宜的连贯性契约。
2. **MIT-SPARK/Spark-DSG**（BSD-2）：分层活场景图（mesh/对象/地点/结构/智能体分层 + 逐节点时间戳）；生态 Hydra（实时构建）/ Khronos（时空）/ Clio（任务驱动剪枝→可借鉴置信度/相关性过滤）。
3. **flecs**（SanderMertens/flecs，MIT）：`world.defer()` 帧末命令缓冲——N 个乱序写入者 → 帧界原子应用、确定顺序。正是我们"多写入者 + 所有权裁决"的实现机制。
4. **EnTT / slotmap**：代际实体 ID（ID+版本位，旧句柄自动失效）；确定性存储顺序迭代。
5. **SpatialOS 存档文档**：组件级权威（某 worker 独占写某组件）——state ownership 措辞的最清晰先例；读文档不抄代码。
6. **virtualhome**：环境图即 canonical state、渲染为下游——我们架构的微缩先例。

## 5. 敲门砖贡献地图（Track B 通道的目标仓库）

原则：贡献点选"适配器/用例/指标/文档"——既被需要又恰好长在我们主线上；PR 进主线前先在自己仓库过好测试。

| 目标仓库 | 贡献点 | 时机 |
|---|---|---|
| AlayaLab/WorldMark | 新模型适配器（按其"证据验收门"标准）/ 指标补充 | Phase 1–2（我们的动作词汇先对齐它） |
| galilai-group/stable-worldmodel | 环境/规划器接口适配、评测案例 | Phase 1–2 |
| meituan-longcat/WBench | 用例包提交 + leaderboard 结果 | Phase 2（我们的 harness 能产出其格式时） |
| NVIDIA/cosmos-evaluator | checker 指标 & 报告接口 | Phase 2–3 |
| OpenEnvision/WorldFoundry | 先盯防其 9 月底大更新，再评估贡献还是差异化 | 更新落地后一周内复盘 |
| MIT-SPARK/Spark-DSG 生态 | 模式学习为主；贡献谨慎（学术仓库节奏） | Phase 3+ |

**Gate 2 双轨认定的实例门**：以上任一仓库的实质性 PR 被 merge，或收到评测团队的合作/讨论邀请，即满足 Track B；外部真实使用/引用满足 Track A。

## 6. 盯防清单（已同步进 research-queue.md）

- **WorldFoundry "comprehensive update"**（预告 2026-09 月底，方向正是 interactive worlds 评测）——落地后一周内做对比 ADR。
- **World-In-World**（ICLR'26 Oral，arXiv 2510.18135）——闭环评测"具身效用"主张的持有者；其统一动作 API 是我们 Action 契约的对齐参照。
- **stable-worldmodel** 的 env/planning 接口演变（2.2k★、最活跃闭环派）。

## 7. 与方向决策报告的关系

- 补强：§4.7 评测生态由 10 家扩至 18 家（新增 World-In-World、Omni-WorldBench、MIND、WNM-3D、CounterScene）；§5 白地结论不变但更硬（负检索 + 两派各缺一半的实证）。
- 修正：无。方向 D 在更多数据下依然成立。
- 新增风险：无；既有风险 #2/#6 的盯防对象具体到仓库（见 §6）。
