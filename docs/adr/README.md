# 架构决策记录（ADR）

本目录是 2026-09-28 起的 ADR 存放处。新 ADR 格式见 [`.agents/skills/domain-modeling/ADR-FORMAT.md`](../../.agents/skills/domain-modeling/ADR-FORMAT.md)，从 **0008** 起编号。

## 本目录 ADR

| 编号 | 决策 | 日期 |
|---|---|---|
| [ADR-0008](0008-direction-reopen-gpu-native-world-engine.md) | 方向重开：GPU 原生世界引擎为本体，评测降为第一应用（R3 融合） | 2026-10-10 |
| [ADR-0009](0009-reproducibility-dual-mode.md) | 可复现性双模式：参考模式逐字节，性能模式容差 | 2026-10-10 |
| [ADR-0010](0010-process-reopen-co-designed-rebuild.md) | 过程重开：白纸共创重建（总分三层设计文档 + 设计先行 + 技能映射） | 2026-10-12 |
| [ADR-0011](0011-contract-rear-mirror-delta-review.md) | 契约后视镜 delta 审查：4 重合 / 3 收紧 / 5 缺口处置；L0 §2 定稿冻结 | 2026-10-12 |

## 既有决策记录索引（在别处，勿重复搬运）

本仓库的重要决策在引入本目录之前已经记录，位置如下——引用时直接指向原始文件：

| 决策 | 记录位置 | 日期 |
|---|---|---|
| 项目方向：评测优先的 grounding-loop runtime（选 D，否决 A/B/C；**定位部分已被 ADR-0008 取代**，架构资产保留） | [`docs/99-archive/2026-09-28-docs-slim/01-architecture/decisions/ADR-0007-project-direction.md`](../99-archive/2026-09-28-docs-slim/01-architecture/decisions/ADR-0007-project-direction.md)；证据：[`docs/17-research/2026-09-21-direction-decision.md`](../17-research/2026-09-21-direction-decision.md) | 2026-09-21 |
| World Contract v0.1 冻结（实体/tick/所有权/置信度语义；世界模型适配器族；回放格式） | [`PLAN.md`](../../PLAN.md) §7 | 2026-09-28 |
| 组件选型与许可红线（自写四处核心；ONNX/RT-DETR/DA-V2-Small/MCAP 归位） | [`docs/17-research/2026-09-24-github-reuse-map.md`](../17-research/2026-09-24-github-reuse-map.md) | 2026-09-24 |
| 双轨成功模型（生态采用 + 个人技能敲门砖；**已被 ADR-0008 三层目标函数取代**） | [`PLAN.md`](../../PLAN.md) §4（已重写） | 2026-09-24 |
| 历史 ADR-0001~0015（模板化旧集：Vulkan/CUDA/Linux/ROS2-adapter/Brain-agnostic 等） | [`docs/99-archive/2026-09-planning-era/decisions/`](../99-archive/2026-09-planning-era/decisions/) 与 [`docs/99-archive/2026-09-28-docs-slim/01-architecture/decisions/`](../99-archive/2026-09-28-docs-slim/01-architecture/decisions/) | 2026-08~09 |

## 何时写新 ADR

- 语义级变更（World Contract 字段语义、冲突策略变更、可复现性承诺变更）——PLAN.md §7 规则。
- 方向级重审（目标函数/Gate 体系变更）——ADR-0008 先例。
- 引入新的重量级依赖或放弃既有选型。
