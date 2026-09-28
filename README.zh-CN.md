# OmniWorld Engine

[English README](README.md)

**OmniWorld 是一个开放运行时：让感知模型、世界模型、仿真器、渲染器、智能体共享同一个可测量的"活世界状态"**（实体身份 / 时间 / 所有权 / 置信度契约）。第一站是交互式世界模型的闭环评测 harness——十几个学术评测团队正在重复手写的那套东西。

## 文档只有三份

| 文件 | 作用 |
|---|---|
| [`PLAN.md`](PLAN.md) | **唯一必读**：是什么 / 为什么 / 路线图与三个 Gate / World Contract v0 / 组件选型 / 许可红线 |
| [`CURRENT.md`](CURRENT.md) | 本周唯一焦点（现在 = Week 2：`src/world_state` + 确定性真值世界） |
| `docs/17-research/` | 证据层：方向决策报告（2026-09-21）、GitHub 复用地图（2026-09-24）、研究队列 |

其余历史文档（旧规划、治理、导读系列、两份研究报告）全部在 `docs/99-archive/`，只查不读。

## 当前状态

- 方向已定案（2026-09-21，评测优先的 grounding-loop runtime），规划阶段关闭。
- 已有代码：`src/runtime_probe`（C++20 硬件探针，2026-09-18 baseline）。
- 下一步：**Week 2 —— `src/world_state` 与确定性真值世界**（见 [`CURRENT.md`](CURRENT.md)）。

## 构建与运行

```bash
cmake -B build -G Ninja && cmake --build build
./build/runtime_probe            # CPU/CUDA/Vulkan 能力探针
./build/runtime_probe --json     # 机器可读输出
ctest --test-dir build           # 冒烟测试
```

要求：C++20 编译器、CMake + Ninja；CUDA 与 Vulkan loader 自动检测（可选）。

## 语言与规则

- `PLAN.md`/`CURRENT.md` 中文优先，术语保留英文；证据层研究报告中文。
- 代码未跑通前不加新文档；新想法进研究队列，不进 sprint。
