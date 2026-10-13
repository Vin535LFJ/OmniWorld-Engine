# Spec — 阶段 A「核心引擎重建」

Status: ready-for-agent

> 来源：L0（已冻结：契约 §2 / 架构 §3 / 阶段表 §4）+ L1（已冻结：模块 / 签名 / 切片）+ ADR-0008/0009/0010/0011。词汇遵循 GLOSSARY.md；设计文档是本 spec 的权威附件。工具无关：任何 AI 按本 spec + L0/L1 可领任务。

## Problem Statement

引擎身份是 GPU 原生世界引擎（ADR-0008），但仓库当前没有任何引擎代码（旧实现按 ADR-0010 作废，仅剩 runtime_probe）。owner 的目标函数（磨练技术 ≳ AI 岗敲门砖）要求以白纸共创方式重建引擎，并以 Gate-1 对齐报告证明新设计重新挣得旧实现的全部已验证证据。

## Solution

按冻结设计（L0 §2 契约 + L1 模块/签名）重建最小闭环：真值世界确定性推进 → 观察生成 → 感知适配器经接缝入环 → 裁决 → 信念账本留痕 → 指标评分。6 个垂直切片逐个走「L2 详细设计 → owner 批准 → TDD 实现」，以 Gate-1 对齐报告收口。

## User Stories

1. As the engine owner, I want a truth world that advances deterministically by tick, so that the same seed reproduces the same world byte-for-byte.
2. As the engine owner, I want WorldState to have no public write interface (single writer = truth world), so that "who corrupted the data" is always answerable.
3. As the engine owner, I want observations generated at any requested resolution with three-state visibility, so that models with different input shapes work unchanged（契约⑧）.
4. As an adapter author, I want to attach a perception model as a single self-registering file, so that the core needs zero per-model code（FR6）.
5. As an adapter author, I want my adapter to see only the Observation and emit only Proposals, so that it structurally cannot corrupt truth（契约④⑤）.
6. As the engine owner, I want every proposal adjudicated（matched / new entity / rejected with reason）and recorded verbatim in the belief ledger, so that mistakes are measurable data, not accidents.
7. As the engine owner, I want confidence to be a mandatory single scalar with zero adjudication power, so that calibration（ECE）catches overconfident models（契约⑦）.
8. As the engine owner, I want pluggable metrics（divergence / ECE / 身份稳定性）fed from truth + ledger + tick log, so that adding a metric never touches the core.
9. As the engine owner, I want a per-tick log with applied/rejected counts and per-stage latency（契约⑨）, so that latency claims have a contract-level carrier.
10. As the engine owner, I want world-model adapters on a second seam predicting next state, so that the contract proves family-agnostic.
11. As a portfolio viewer, I want the Gate-1 alignment report comparing the new implementation against the `pre-rebuild` baseline（1106.7 Hz / 适配器零核心改动 / ECE 判别）, so that the rebuild's claims are independently verifiable.

## Implementation Decisions

- 10 个模块与冻结容器图 1:1（L1 §2）；所有跨模块依赖单向汇入 loop，无环。
- 接缝与核心的 C++20 签名按 L1 §4（PerceptionAdapter / WorldModelAdapter / AssociationStrategy / BeliefLedger / MetricsModule / LoopDriver）。
- 模式出处与反模式纪律见 L1 §8（端口与适配器 / 管线 / 单一作者追加账本 / 策略 / 注册表 / 深模块 / 值对象）。
- 语言策略：C++20 "boring core"（不用 modules/coroutines）；零外部依赖；参考模式逐字节复现（ADR-0009 参考模式 = 唯一契约回归 oracle）。
- 裁决 v1 策略 = 最近位置 + 类别匹配，一种即可（usage-driven）。
- 出错处理：格式错误便签拒收+原因留痕；关联失败=拒绝非异常；适配器异常不得阻断 tick 推进。
- 每切片动工前必须有经 owner 批准的 L2 详细设计（PLAN 规则 #6，硬 gate）。

## Testing Decisions

- 只测契约可见行为：逐字节重放、适配器不变式（新适配器 = 单文件、git diff 证核心零改动——damped_linear 先例）、裁决三分支、ECE 判别（故意注入错误置信度的适配器必须被区分——Gate 1c 同构）、TickLog 字段齐全。
- 不断言内部实现；参考模式测试是唯一 oracle（性能模式判据属阶段 B）。
- Prior art：`pre-rebuild` 两份 regression 报告的格式（环境元数据 + 可复现命令）。

## Out of Scope

双模式 CLI 与容差框架（阶段 B）；ONNX 真模型（阶段 C）；Vulkan 渲染器（可选技能项目）；Action（契约预留）；仲裁策略族；付费大算力；ROS 2/VLA/LLM、分布式、编辑器、商业化。

## Further Notes

- 切片顺序与依赖见 L1 §5（01→02→03→04→05→06 线性链）；06 出口 = 阶段 A 完成。
- 实现中问题随遇随记 `docs/design/problem-log.md`（现象→定位→决策→教训，STAR 素材）。
