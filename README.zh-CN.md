# OmniWorld Engine

[English README](README.md)

## 🚧 当前开发焦点

Phase: **Phase 1 — GPU Runtime Foundation**

Current Goal: 构建第一个可运行的 **OmniWorld Runtime Probe**，输出 CPU、GPU、CUDA、Vulkan、VRAM、Driver 和 runtime configuration。

This Week: 创建 C++20/CMake runtime skeleton，实现 CUDA/Vulkan capability probe，加入 logging/metrics，并记录第一份可复现 probe 输出。

Status: 当前已从“规划很多”收敛为“只推进一个 ACTIVE 项目”。

Next Milestone: `runtime_probe` 可以从 clean checkout 构建并运行，输出可复现的环境与 GPU 能力报告。

## 项目定位

OmniWorld Engine 是一个 GPU 原生实时 Runtime，目标是通过统一的 World Representation 连接视频/传感器、GPU compute、AI inference、Vulkan rendering、World State、Brain/Agent、ROS 2/Simulation 和未来的 VLA/Physical AI。

## 当前唯一主线

```text
GPU Runtime
     ↓
GPU Video
     ↓
Vulkan Renderer
     ↓
AI Runtime
     ↓
World State
     ↓
Brain / Agent
     ↓
ROS2 / Simulation
     ↓
VLA / WAM
```

当前只推进：**GPU Runtime Foundation**。

## 现在不要做

不要同时学习或实现 ROS 2、CARLA、Isaac、VLA、World Model training、Digital Human、完整物理引擎、完整 Game Engine、移动端或分布式系统。这些方向保留在 Backlog 或 Research Queue 中。

## 开发入口

- 每日入口：[`CURRENT.md`](CURRENT.md)
- 总路线：[`docs/02-roadmap/development-plan.md`](docs/02-roadmap/development-plan.md)
- 当前 Sprint：[`docs/02-roadmap/current-sprint.md`](docs/02-roadmap/current-sprint.md)
- 30/60/90 天计划：[`docs/02-roadmap/30-60-90-day-plan.md`](docs/02-roadmap/30-60-90-day-plan.md)
- 不要现在做：[`docs/02-roadmap/not-now.md`](docs/02-roadmap/not-now.md)
- Backlog：[`BACKLOG.md`](BACKLOG.md)
- 执行指南：[`docs/PROJECT_EXECUTION_GUIDE.md`](docs/PROJECT_EXECUTION_GUIDE.md)
- 工程生命周期：[`docs/03-engineering-lifecycle/README.md`](docs/03-engineering-lifecycle/README.md)

## 工作方式

```text
打开 CURRENT.md
        ↓
确认本周唯一目标
        ↓
学习当前代码需要的知识
        ↓
写代码
        ↓
实验
        ↓
Benchmark
        ↓
Weekly Review
```

## 语言规则

- 项目架构文档：英文优先。
- 学习知识库：中文优先。
- 技术名词：保留英文原文，并在必要时补充中文说明，例如 CUDA Stream（CUDA 流）。
