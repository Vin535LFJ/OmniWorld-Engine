# OmniWorld Engine

[English README](README.md)

**OmniWorld 是一个 GPU 原生世界引擎（C++20）：让世界数据——视频解码、AI 推理、观测渲染、世界状态更新——以最少实际拷贝、最少同步、最低延迟穿过 GPU 系统。** 感知模型、世界模型、仿真器、渲染器、智能体共享同一个可测量的"活世界状态"（实体身份 / 时间 / 所有权 / 置信度契约）；评测 harness 是引擎的第一个应用（ADR-0008）。

## 文档

核心三份 + 支撑层：

| 文件 | 作用 |
|---|---|
| [`PLAN.md`](PLAN.md) | **唯一必读**：定位 / 目标函数 / 里程碑路线图与 Gate 体系 / World Contract / 许可红线 |
| [`CURRENT.md`](CURRENT.md) | 本周唯一焦点与出口条件（现在 = Phase 2「AI 入环」） |
| [`GLOSSARY.md`](GLOSSARY.md)、`docs/adr/`、`docs/17-research/`、`docs/agents/` | 支撑层：术语、决策记录、证据、约定 |

其余历史文档在 `docs/99-archive/`，只查不读。

## 当前状态

- Phase 0 + Phase 1 已完成（2026-09-28）：Gate 1 三判据 PASS——闭环 1106.7 Hz、第二模型接入核心零改动、ECE 判别分离 0.9617；五个世界模型经同一契约入环，divergence 五级分离。
- 方向于 2026-10-10 重开定案（ADR-0008：GPU 原生世界引擎为本体）。
- 下一步：**Phase 2「AI 入环」——ONNX Runtime 真模型换装**（见 [`CURRENT.md`](CURRENT.md)）。

## 构建与运行

```bash
build.bat --test                # Windows 唯一可靠入口：configure + build + ctest
build\omniworld_loop.exe        # Grounding Loop v0 + Gate 1（参考模式，逐字节可复现）
build\omniworld_loop.exe --mode phase1   # 世界模型 divergence 评测
```

要求：MSVC + CMake（`build.bat` 自动配置）；参考模式零外部依赖、同 seed 逐字节复现；性能模式容差复现（ADR-0009）。

## 语言与规则

- `PLAN.md`/`CURRENT.md` 中文优先，术语保留英文；证据层研究报告中文。
- 事实与主张分离：每个技术论断必须可追溯到仓库证据或外部原始来源；没有 benchmark 数据不宣称性能。
- 代码未跑通前不加新文档；新想法进研究队列，不进 sprint；方向/契约级决策落 ADR。
