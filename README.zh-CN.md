# OmniWorld Engine

[中文说明 / Chinese README](README.zh-CN.md)

**OmniWorld 是一个 GPU 原生世界引擎（C++20）：让世界数据——视频解码、AI 推理、观测渲染、世界状态更新——以最少实际拷贝、最少同步、最低延迟穿过 GPU 系统。** 感知模型、世界模型、仿真器、渲染器、智能体共享同一个可测量的"活世界状态"（identity / time / ownership / confidence 契约）；评测 harness 是引擎的第一个应用（ADR-0008）。

## 文档地图（从哪看起）

| 顺序 | 文档 | 作用 | 读完你知道 |
|---|---|---|---|
| ① 5 分钟 | 本 README | 项目是什么 + 文档地图 | 项目一句话 + 去哪找什么 |
| ② 15 分钟 | [`PLAN.md`](PLAN.md) | **唯一必读**：定位 / 目标函数 / 里程碑路线图与 Gate 体系 / 操作规则 | 项目要做成什么、规矩是什么 |
| ③ 1 分钟 | [`CURRENT.md`](CURRENT.md) | 现在做什么 + 出口条件 | 今天该打开哪个文件 |
| ④ 20 分钟 | [`docs/design/00-总体设计.md`](docs/design/00-总体设计.md) | **当前权威设计（已冻结）**：World Contract 契约语义 + 总体架构 + 阶段划分 | 契约怎么定的、系统长什么样 |
| ⑤ 按需 | [`docs/design/`](docs/design/) 各阶段目录 | 概要设计（L1）/ 切片详细设计（L2）/ [`problem-log.md`](docs/design/problem-log.md) 踩坑记录 | 某一块具体怎么设计、踩过什么坑 |
| ⑥ 按需 | [`docs/adr/`](docs/adr/) | 每个重大决策"为什么"：0008 方向重开 / 0009 双模式 / 0010 白纸重建 / 0011 后视镜 | 决策的来龙去脉 |
| ⑦ 按需 | [`GLOSSARY.md`](GLOSSARY.md) · [`benchmarks/regression/`](benchmarks/regression/) | 术语表 · 性能主张的唯一证据 | 概念的精确定义 / 数字出处 |

## 当前状态（2026-10-13）

- **白纸共创重建中**（ADR-0010）：旧实现已作废（`git tag pre-rebuild` 可查）；按"设计先行"流程重建——L0 总体设计已冻结（契约 9 条语义 + 架构 + 阶段表），阶段 A 概要设计（L1）已产出待批。
- 当前可构建代码仅 `runtime_probe`（硬件探针，独立）；引擎随阶段 A 切片逐片回归。
- 旧基线（重建对照）：Gate 1 三判据 1106.7 Hz / 适配器零核心改动 / ECE 分离 0.9617。
- 开发流程 = AI Hero 主流程：`grill-with-docs → to-spec → to-tickets → implement+tdd → code-review → retro`（技能映射见 ADR-0010）。

## 构建与运行

```bash
build.bat --test                # Windows 唯一可靠入口（当前 = runtime_probe 冒烟测试 1/1）
build\runtime_probe.exe --json  # 硬件能力探针（CPU / CUDA / Vulkan）
```

`omniworld_loop.exe`（Grounding Loop）将随阶段 A 切片回归后重新提供。

## 规则

- **设计先行**：任何代码切片动工前，必须有经 owner 批准的详细设计（L2）。
- 事实与主张分离；没有 benchmark 数据不宣称性能；方向/契约级决策落 ADR。
- 代码未跑通前不加新文档；新想法进研究队列，不进 sprint。

## 语言

`PLAN.md` / `CURRENT.md` / 设计文档中文优先（术语保留英文）；研究报告中文。
