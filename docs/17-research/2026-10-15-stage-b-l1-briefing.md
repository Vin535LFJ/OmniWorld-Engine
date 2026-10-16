# 阶段 B L1 共创简报——双模式框架（2026-10-15）

> **声明**：本简报只收事实与选项，一切设计决定由 owner 在阶段 B L1 共创中裁决。
> **来源与核对**：三路考察（双模式考古 / 契约提炼 / 场景先例）各由考察员产出事实与开放问题，再各由独立复核员逐条核对出处（现行路径用 Read、`pre-rebuild` 用 git show），成立进 confirmed、不成立进 rejected——交付本文时三路 rejected 均为空，本文未采用任何被否决内容。考察规格与流程见本机工作流文件 `.zcode/workflow-drafts/阶段B简报考察.dwf.ts` 与 `.zcode/workflow-runs/dwfrun-3841826f-79a0-471a-9c93-188e22b65632.mjs`（git 忽略、未入库）；三路结果原文由工作流运行期传递，未在仓库内落盘，是否归档见 ⑤Q8。
> **出处约定**：带 `pre-rebuild:` 前缀 = 仅存在于旧实现 tag（读法 `git show pre-rebuild:<路径>`）；无前缀 = 现行工作树路径。2026-09-28 回归报告与 JSON 工件在现行树 `benchmarks/regression/` 有同路径副本，本文一律引现行树副本。
> **运行记录（2026-10-16 Q8 裁决补记）**：工作流运行 `dwfrun-3841826f-79a0-471a-9c93-188e22b65632`（三路平行考察 + 链式独立复核）：confirmed 24 / rejected 0、13 个开放问题；全部 confirmed 事实已随本简报 §②③④ 落盘，rejected 为空故无内容需另行归档。8 个开放问题的裁决轨迹见 [`docs/design/stage-b-dual-mode/概要设计.md`](../design/stage-b-dual-mode/概要设计.md) §2。

## ① 阶段 B 是什么

- **PLAN §六 原句**（过程重开链路，ADR-0010）：「旧实现作废，白纸共创重建——阶段 A 核心引擎重建（出口 = Gate-1 对齐报告，vs `pre-rebuild` 基线）→ **阶段 B 双模式框架** → 阶段 C AI 入环（即本表 Phase 2）」（PLAN.md:82）。§六里程碑表格内没有独立的阶段 B 行。
- **L0 阶段划分表**（已冻结）：目标「ADR-0009 运行层落地」；交付物「L1 + L2（双模式 CLI、容差比较器、延迟计量）」；退出判据「参考模式逐字节不变（ctest 全绿）+ 性能模式容差自证（噪声 stub）」（docs/design/00-总体设计.md:194）。
- **流程约束**：每阶段开工前才做该阶段的 L1（不预支）；切片清单由 to-tickets 从 L1 生成（docs/design/00-总体设计.md:197）。
- **排期入阶段 B 的修补**（对齐报告 Findings 表）：F1「聚合 ECE 掩盖生产者结构 → 需要 per-producer ECE（旧报告分表的教训）」（Metrics 增量；problem-log 005）；F2「无墙常速真值世界使观察窗口后段枯竭 → 对齐运行需有界场景（墙/回绕/场景配置）」——报告只对 F2 点名候选方向，故 ④ 只为 F2 列选项。F3 与二者不同：「已校准『正常模型』席位空缺 → 由阶段 C 真实模型（ONNX）关闭 (c) 的在环部分」——判据 (c) 的在环部分判 PARTIAL，直接原因是面板里两个内建检测器的置信度都是 v1 占位公式、缺一个已校准的正常模型作对照，而该席位按设计留给阶段 C 的真实模型，故 F3 不排阶段 B。报告结论：「随后进入**阶段 B 双模式框架**（含 F1/F2 修补）」。（benchmarks/regression/2026-10-14-stage-a-gate1-alignment.md:52,54,81,89-91,95；docs/design/problem-log.md:39）

## ② 契约级硬性要求（已确认事实）

1. **双模式定义（ADR-0009）**：参考模式 = 纯 CPU、保持逐字节复现、契约回归的唯一 oracle、CLI 默认；性能模式 = GPU 推理及后续 GPU 组件，承诺「固定 seed + 固定输入 → 容差窗口内输出 + 延迟预算达标」，回归判据为容差比较而非字节比较，CLI 以模式开关区分（docs/adr/0009-reproducibility-dual-mode.md:9-10）。
2. **ADR-0009 Consequences**：性能模式数字可发表，但报告必须标注模式开关 + 完整环境元数据；World Contract v0.1 字段语义零改动（本 ADR 只定义运行模式层）；后续每个 GPU 组件（P3 NVDEC、P4 CUDA）落地时必须同时声明其容差窗口（同文件 :14-16）。
3. **契约⑨ Tick 日志**：每 tick 一条日志——applied / rejected（含拒绝原因）+ 逐阶段延迟；是 FR3 数据可计量的契约载体与「性能模式延迟数字的可信度来源」；§2 为契约唯一权威文本，⑨ 属后视镜补充（ADR-0011）且已生效（docs/design/00-总体设计.md:33,47,60）。
4. **契约⑧增补·帧元数据清单**：观察元数据至少含 backend / device / format / dims——这四项均属 v0.1 九字段内存契约的帧 buffer 元数据（该契约的帧元数据为 backend/device/format/dims/pitch/lifetime/sync/timestamp，pre-rebuild:PLAN.md:94），⑧ 增补即从中取观察层必需的四项子集；ADR-0009 要求性能模式如实标注 GPU 后端（docs/design/00-总体设计.md:61）。
5. **延迟计量语义边界（契约③ + FR3/FR4）**：单一 tick 单调时钟域，墙钟仅作性能诊断（延迟计量）、不参与因果；FR3 要求拷贝次数、同步点、逐阶段延迟可测；FR4 要求参考模式逐字节复现、性能模式容差复现 + 延迟预算（docs/design/00-总体设计.md:21-22,54）。
6. **既有确定性实践「时间不进哈希」**：对齐报告面板 B（该报告三运行面板之一：仅 blob_detect 单检测器，:22）两次运行墙钟 0.0125 vs 0.0105 s，而 world_hash / ledger_hash 逐位一致（benchmarks/regression/2026-10-14-stage-a-gate1-alignment.md:22,68）。

## ③ 旧实现先例（代码出处均带 `pre-rebuild:` 前缀；2026-09-28 回归证据引现行树同路径副本）

1. **逐阶段计时**：库内逐 tick 用 std::chrono::steady_clock 对每阶段单独计时，原始微秒样本存 `LoopResult::stage_us`（key：truth_step、render、detect:<name> / adjudicate:<name>）；端到端 loop_hz = ticks/total_wall_s 另以整循环墙钟计算，与逐阶段表相互独立（pre-rebuild:src/grounding_loop/loop.cpp:41-65,119-122；pre-rebuild:src/grounding_loop/loop.hpp:5,80）。
2. **分位数在 runner 层**：`percentile()`（排序后线性插值）在打印「Per-stage latency」表与写 JSON 时调用；工件 stages 每阶段只落 p50_us / p95_us 两个聚合值、不存原始样本——实测工件背书：grounding-loop-report.json 的 stages 共 8 个阶段条目（truth_step、render、3 个 detect:*、3 个 adjudicate:*），每条目仅两聚合字段（pre-rebuild:apps/omniworld_loop.cpp:23-31,178-184,244-252；benchmarks/regression/artifacts/2026-09-28-grounding-loop-v0/grounding-loop-report.json:5-14）。
3. **按模型分表（Phase 1 评测）**：ModelEval 含 fit_s 与 predict_p50_us / predict_p95_us；fit 逐模型计时、predict 逐 tick 逐模型采样后统一取分位数，报告一模型一行（pre-rebuild:src/grounding_loop/phase1.hpp:23-32；pre-rebuild:src/grounding_loop/phase1.cpp:76-104,132-137；benchmarks/regression/artifacts/2026-09-28-grounding-loop-v0/phase1-report.json:4-10）。
4. **按 producer 分表（F1 的直接先例）**：ProducerMetrics 以 producer 名为首字段，loop 内按适配器名累积、按名排序输出；console 表与 JSON producers 数组均为一 producer 一行/一对象；工件 3 个 producer 各带独立 ece（0.0115388 / 0.973207 / 0.595596）（pre-rebuild:src/grounding_loop/loop.hpp:24-26；pre-rebuild:src/grounding_loop/loop.cpp:37,68-69,133；pre-rebuild:apps/omniworld_loop.cpp:186-197,253-264；benchmarks/regression/artifacts/2026-09-28-grounding-loop-v0/grounding-loop-report.json:15-19）。
5. **ECE 实现细节**：封装在 ProducerMetrics 内：5 个 [0,1] 等宽 bin，ece() 为按 bin 加权的 |bin 平均置信度 − bin 经验正确率| 之和；binning 的「good」判据硬编码为「正确 id 且 transform error ≤ 0.05」；首版高置信错误率指标对欠自信反转模型有盲区，故换用标准 5-bin ECE（pre-rebuild:src/grounding_loop/loop.hpp:35-55；pre-rebuild:src/grounding_loop/loop.cpp:100-110；benchmarks/regression/2026-09-28-grounding-loop-v0.md:68）。
6. **Gate 1c 程序化判据先例**：runner 取两 producer（blob_cc 与 blob_cc_miscalibrated）算 discrim = bad − cc 的 ECE 差值，及格线 ≥ 0.30，未过进程返回码 2；实测分离 0.9617（工件 c_discrimination=0.961668，verdict=PASS）（pre-rebuild:apps/omniworld_loop.cpp:210-220,276；benchmarks/regression/artifacts/2026-09-28-grounding-loop-v0/grounding-loop-report.json:20；benchmarks/regression/2026-09-28-grounding-loop-v0.md:58）。
7. **双模式未进旧代码**：「参考/性能双模式」在 pre-rebuild 停在契约与票据层——`git grep -n -i -E "reference.?mode|performance.?mode|参考模式|性能模式" pre-rebuild -- "*.cpp" "*.hpp" "*.bat" CMakeLists.txt tests apps src` 零匹配（exit 1）；CLI 既有 --mode 只区分 phase0|phase1；落地被开成 phase2 票据 03（验收项含「性能模式数字必须可携带模式 + 环境元数据标注」）（docs/adr/0009-reproducibility-dual-mode.md:9-10,14,16；pre-rebuild:apps/omniworld_loop.cpp:37,51；pre-rebuild:.scratch/phase2-ai-in-loop/issues/03-dual-mode-cli-tolerance-framework.md:3,9-12）。
8. **旧报告的模式标注实态**：两份 2026-09-28 回归报告（早于 ADR-0009 的 2026-10-10）全文无「模式」字样（本轮修订通读两文核实）；环境靠「Environment（完整元数据）」表标注，GPU 行写明「本轮未用 CUDA——v0 闭环为纯 CPU」（benchmarks/regression/2026-09-28-grounding-loop-v0.md:7-19；2026-09-28-phase1-world-models.md:7-9，环境表为对前者的引用）；两份 JSON 工件均无 mode / environment 字段（grounding-loop-report.json 与 phase1-report.json 本轮全文核对）。而 pre-rebuild 的 PLAN.md 已要求「报告必须带完整环境元数据 + 运行模式标注（参照 2026-09-18 baseline 格式）」（pre-rebuild:PLAN.md:108,120；现行 PLAN.md:111 同款），现行 GLOSSARY 对回归报告的定义亦同款（GLOSSARY.md:95-97）；逐字节承诺由 ow_tests determinism 测试背书（benchmarks/regression/2026-09-28-grounding-loop-v0.md:64；测试代码 pre-rebuild:tests/ow_tests.cpp:28-58）。
9. **弹墙机制本体**：每 tick 先 pos += vel，再对 x/y 两轴各做一次镜像反射并取反该轴速度（pos.x < r 时 pos.x = r+(r−pos.x)、vel.x = −vel.x；对侧与 y 轴同构）；if/else-if 结构、每轴每 tick 至多一次、无循环 clamp；核心块 16 行（pre-rebuild:src/truth_world/truth_world.cpp:61-77）。
10. **弹墙确定性**：step() 内零 RNG 调用，反弹为纯浮点算术（镜像反射 + 速度取反），同 seed 位级可复现；Pcg32::seeded(seed, 0xDA3E5CBF) 仅 init 用一次；头文件注释明示 "Deterministic seeded kinematic truth world... elastic wall bounces"；v0 无实体间碰撞（pre-rebuild:src/truth_world/truth_world.hpp:2-5；pre-rebuild:src/truth_world/truth_world.cpp:8,56-83）。
11. **旧 init 参数域**：半径 ∈ [0.018, 0.045]、出生位置在 [margin, 1−margin] 拒绝采样 ≤200 次（最小间距 r1+r2+0.05；200 次全败无失败处理，b.pos 静默保持零初始化 (0,0)）、速度 ∈ [0.004, 0.020]/tick——单次反射必落界内依赖该速度上界（≤0.020 远小于半域宽）（pre-rebuild:src/truth_world/truth_world.cpp:17-42；pre-rebuild:src/core/types.hpp:14-16）。

## ④ 有界场景候选选项与权衡（F2；列选项，不裁决）

> **为何只覆盖 F2**：对齐报告只对 F2 点名三个候选方向（benchmarks/regression/2026-10-14-stage-a-gate1-alignment.md:81,90）；F1 在已确认材料中无候选菜单，已知决策点即 ⑤Q1/Q2。problem-log 005 载「Metrics 深模块公开接口已就位，纯增量」（docs/design/problem-log.md:39）——本轮代码级抽查与之一致：MetricsModule 公开面 = 可插拔 metric（register_metric）+ report() 平铺 map<string,double>（tests/ow_tests.cpp:715-721,783-793），账本条目携带 producer 名（:630-649）。若共创中发现 F1 另有形态选择，届时补列。

**现状前提**（已确认）：

- 当前无边界：WorldState::step() 为纯常速积分（x/y/z 各 += velocity 分量，无任何边界分支）；TruthWorld::advance 只循环 ws.step() 到目标 tick——旧架构「TruthWorld 自持 bodies_ + commit_truth 写回」路径已不存在（src/world_state/world_state.cpp:26-33；src/truth_world/truth_world.cpp:12-17）。
- 当前 demo 出生速度分量 ∈ (−0.05, +0.05)/tick（(next_unit()−0.5)×0.1），与 F2「数十 tick 内飘出观察窗口」量级一致；当前 Rng 为 splitmix64，next_unit 取高 24 位且可被 float 精确表示（src/truth_world/truth_world.cpp:19-27；src/core/rng.hpp:2,19-22）。
- Entity 只有 id / class_tag / transform / velocity，无 radius / extent——旧弹墙以每体 r 为界，移植需补边界量；WorldState 公共面只读、唯一写者是 TruthWorld（friend 通道 mint/step），头文件告示「A future second writer must amend this one visible place and go through an ADR」（src/world_state/world_state.hpp:2-32）。
- 观察窗口 = x/z 平面 [0,1]²，出窗世界坐标 clamp 到边缘像素（world_to_pixel）；可见性三态（完全在窗内→Seen、完全出窗→Unseen、骑墙→Uncertain），视觉尺寸来自渲染配置 footprint 而非实体属性——飘出窗口的实体表现为 Unseen 而非消失报错（src/observation/observation.cpp:26-79）。

**候选选项**（对齐报告 F2 已点名三方向，benchmarks/regression/2026-10-14-stage-a-gate1-alignment.md:81-90）：

| 选项 | 内容 | 利 | 代价/风险 |
|---|---|---|---|
| **A 弹墙镜像反弹**（复刻 pre-rebuild） | 镜像反射 + 速度取反 | 反弹 tick 给世界模型/裁决注入结构化、原则上可学的预测误差（利于 Gate-1 (c) 与 F1 校准，机制见下） | 须在唯一写者通道内实现；实体无 radius——边距要么常数（与渲染 footprint 脱钩，可能「看似压墙却已反弹」）、要么给 Entity 加 extent 字段（状态结构 + 哈希/账本连带变更，波及面见下） |
| **B 回绕（torus wrap，pos −= floor(pos)）** | 边界处坐标回绕 | 代码最少、无速度不连续、纯算术确定性同 A | 边界处位置瞬移对所有世界模型都不可预测，会给账本/ECE 灌入与「模型差异」无关的场景误差，且视觉连续性断裂——误差该源自模型还是场景，决定 wrap 是信号还是噪声 |
| **C 场景配置化** | bounds 策略 none/wall/wrap 作为 RunConfig 新增字段（无默认值） | 可保留无界行为做阶段 A / pre-rebuild 基线并排对照——链条：none 保留使基线场景（无墙）在新代码仍可复现，而 ADR-0009 性能模式承诺以「固定 seed + 固定输入」为前提（②1），硬编码墙会让基线输入消失、对照失去共同输入 | 配置面与测试组合扩张；每策略需独立确定性配对测试；v1 一次到位枚举还是先装单一策略本身是决策 |

**机制对照（A 的「利于 F1 校准」何以为证）**：反弹律确定且原则上可学——学得会反射律的模型边界 tick 预测准、学不会者错，误差随生产者能力分离，per-producer ECE 才有「按生产者区分置信度 vs 实际正确率」的信号；旧 Phase 1 报告是实测先例：linear 外推「自由段精确、墙反弹错」、MLP「学到 delta≈vel，未学到反弹」，五模型 divergence 两两可分、墙附近质性差异被指标显式化（benchmarks/regression/2026-09-28-phase1-world-models.md:20-25）。B 反之：瞬移对所有模型同等不可预测，误差与「模型差异」无关，ECE 分不出生产者结构。

**横切决策点**（任一选项都需回答）：

- **实现落点**：反弹/回绕逻辑写进 WorldState::step 本体（src/world_state/world_state.hpp:31-32，现注释 "constant-velocity integration"）——接口零改动，TruthWorld 调用面不变（advance 仍只调 ws.step），代价是 WorldState 从纯积分器语义变宽；或逻辑放 TruthWorld 一侧——写者仍是唯一写者（friend 声明，world_state.hpp:29），但现行 friend 通道只有 mint/step 两个私有入口（:31-32），实现反射要么为 WorldState 新增读写成员、要么经 friendship 直接操作私有字段（entities_，:36）——两者都改动了头文件 :2-5 告示所指「唯一可见位置」的写面定义。该告示字面只管「未来第二写者须走 ADR」（:4-5）；为既有唯一写者扩写面是否同样须走 ADR，文本未覆盖——考察材料标「触碰告示」即指此歧义，裁定权在 owner。
- **注释-实现偏差**：旧代码「注释写 with clamp 但无独立 clamp」，且单次反射的界内不变式（每轴每 tick 恰一次反射）依赖速度上界而无测试守护——移植时补 clamp / 不变式测试修正偏差，或仅照搬原样（接受潜在文档偏差）。
- **边界事件口径**：反弹/回绕 tick 的位置不连续是否作为一等记录进 TickLog / 账本——直接影响 F1 per-producer ECE 的分组方式（场景事件误差 vs 模型误差），也决定对齐运行判别力的解读。

**④A 边界量与 Entity extent 的参数参照**（本轮代码级核实）：

- 常数边距的已确认锚点：旧反弹按每体半径，域 [0.018, 0.045]（pre-rebuild:src/truth_world/truth_world.cpp:17-42）；现渲染 footprint 默认 0.02（apps/omniworld_loop.cpp:36，CLI `--footprint` 可覆盖 :50-51；对齐运行同值，2026-10-14 报告 :22；RenderConfig 成员 observation.hpp:46，非正值抛错 observation.cpp:43-44）。常数建议值本身是设计决定，留共创裁决。
- Entity 加 extent 字段的波及面：生产哈希 hash_world 逐实体枚举 id / class_tag / transform / velocity（src/loop/driver.cpp:27-38），测试内另有同构本地副本两处（tests/ow_tests.cpp:38-49,501-512）——新字段进哈希须改这三处枚举；账本哈希 hash_ledger 只覆盖 proposal / verdict 字段（driver.cpp:40-58），extent 不经 Proposal 则账本哈希不受影响；现行全部哈希断言为关系型（同 seed 自比 / 异 seed 必异）、无硬编码哈希基线（通读 tests/ow_tests.cpp 核实），故确定性初始化的新字段不会使既有 ctest 断言失败——真正失效的是与 2026-10-14 对齐报告记录哈希值（world_hash=574361e62eec92b9 / ledger_hash=70ed1958194a1cad，:68）的跨版本对照。

## ⑤ 留给 owner 的开放问题清单（三路 openQuestions 合并去重 + 修订第 1 轮新增）

> 标注【L1 前】/【可晚】是撰稿人建议的裁决时点（依据：切片清单由 to-tickets 从 L1 生成，docs/design/00-总体设计.md:197——凡决定模块边界、接口面、契约结构或切片构成者建议 L1 前裁决；纯数值与实现细节可留切片内）；划分本身待 owner 裁决。

1. **p50/p95 分位数的计算层**【L1 前：定库接口面】：沿用旧例由 runner 在打印/落盘时算（LoopResult 只存原始样本），还是下沉进库供 gate 程序化读取？——权衡：原始证据保真 vs 自动判读便利。
2. **ECE「good」判据与 Gate 阈值是否重定**【L1 前：判据形态；阈值数字可留切片内】：旧硬编码「正确 id 且误差 ≤ 0.05」与 Gate 1c 阈值（ECE 分离 ≥ 0.30、p50/p95 分位数口径）在新实现原样沿用，还是随置信度语义重定？契约⑦ 原文：置信度 = 生产者自报的「主张正确概率」、强制单一标量 [0,1]、零裁决权力、唯一消费者 = 校准指标 ECE——即置信度只流向 ECE、不流向裁决（docs/design/00-总体设计.md:58）。——权衡：与旧数字可比性 vs 语义正确性。
3. **模式 + 环境元数据是否进机器可读工件**【L1 前：定工件 schema】：旧例只标在 Markdown 报告层、JSON 工件无该字段（③-8）；新报告若要自动校验「已标注模式」就必须写进 JSON——权衡：ADR-0009 原文只约束「报告」层（②2）。
4. **性能模式延迟预算的挂点与初值**【L1 前：挂点定 schema；数值可切片内】：挂点无旧代码先例——pre-rebuild spec 规划过「WorldTransition 的 per_stage_latency 字段 + 帧元数据标注 backend/device」，但该字段两棵树都从未存在（本轮修订实测：`git grep -n -E "per_stage_latency|WorldTransition" pre-rebuild -- "*.cpp" "*.hpp"` 与去掉 refspec 的同命令均零匹配，exit 1）。数值层面：已确认材料与票据 03 中均无任何已定的延迟预算 / 容差窗口数值——ADR-0009:16 只规定「落地时声明」义务，票据 03 只有机制设计（容差比较器 + 延迟预算断言），初值需在阶段 B 从零定。预算与标注落帧元数据、工件 JSON，还是仅报告层？
5. **「噪声 stub」的定性与本意**【L1 前：定自证范围与切片构成】：退出判据「性能模式容差自证（噪声 stub）」在已读文档中无进一步定义。术语唯一有案可查的前身是 pre-rebuild 票据 03 的设计：「注入微小浮点噪声的 stub 适配器」——容差比较器的自证工具（噪声在窗口内 → 通过；超窗 → 判红），不依赖真模型 / ONNX Runtime（pre-rebuild:.scratch/phase2-ai-in-loop/issues/03-dual-mode-cli-tolerance-framework.md:3,10）。L0 退出判据与该票据同源的可能性高，但是否等同采纳、以及是否构成 ADR-0009:16 意义上须声明容差窗口的「GPU 组件」，须 owner 回顾裁定。
6. **PLAN §六 两处文字是否同步修订**【可晚：文档维护，不阻塞切片】：Phase 2 表格行仍列「双模式回归框架（ADR-0009）」为 Phase 2 内容（PLAN.md:77），而同节重开链路把双模式框架独立为阶段 B（PLAN.md:82）——权衡：保留旧表原文作历史 vs 消除歧义。
7. **F2 修复形态三选一**【L1 前：定模块边界与 Entity 结构；clamp/不变式等实现细节可留切片内】：墙（④A）/ 回绕（④B）/ 场景配置（④C）未裁定（对齐报告只列选项，benchmarks/regression/2026-10-14-stage-a-gate1-alignment.md:81,90）——权衡：场景真实性 vs 与 2026-10-14 基线（8 实体、无墙、64×64）速率数字的可比性。连带子决策：④A 的边距取常数还是给 Entity 加 extent 字段（波及面见 ④ 参数参照）；实现落点（见 ④ 横切决策点）；移植时补 clamp/不变式测试还是照搬原样；边界事件是否进 TickLog/账本口径（影响 F1 ECE 分组）。
8. **三路考察底稿是否落盘归档**【可晚：不阻塞切片】：三路结果原文现仅由工作流运行期传递（见文件头「来源与核对」），仓库内无独立底稿文件——是否将其落盘为可核对归档（如 docs/17-research/ 或 .scratch/）由 owner 定。
