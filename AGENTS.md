# OmniWorld Engine — Agent 指引

**这是一个 GPU 原生世界引擎，评测是第一个应用**（ADR-0008）：让世界数据以最少拷贝/同步/延迟穿过 GPU 系统；感知模型、世界模型、仿真器、渲染器、智能体共享一个可测量的活世界状态（World Contract：identity / time / ownership / confidence）。当前阶段与全部事实以 **[PLAN.md](PLAN.md)** 为唯一必读来源；本周做什么看 **[CURRENT.md](CURRENT.md)**。

## 项目事实（开工前先读）

1. [PLAN.md](PLAN.md) — 定位/目标函数/里程碑路线图与 Gate 体系/组件选型/操作规则（World Contract 权威文本已迁至 `docs/design/00-总体设计.md` §2，PLAN §七 仅为指针）。
2. [CURRENT.md](CURRENT.md) — 本周唯一焦点与出口条件（"现在做什么"的**唯一**来源）。
3. [GLOSSARY.md](GLOSSARY.md) — 领域术语（讨论与命名必须使用其中词汇，见 `docs/agents/domain.md`）。
4. [docs/adr/](docs/adr/) — 架构决策记录（含既有决策索引）。
5. `benchmarks/regression/` — 全部性能与指标主张的唯一证据来源。
6. [docs/agents/workflow-manual.md](docs/agents/workflow-manual.md) — **工作流操作手册**：各环节职责/输入输出/衔接、SOP、状态机、Definition of Done、反模式清单。日常开工直接照此执行。

## 构建与测试

```bash
build.bat --test                # MSVC 环境下 configure + build + ctest（Windows 唯一可靠入口）
build\runtime_probe.exe --json  # 硬件能力探针（CPU / CUDA / Vulkan）
# omniworld_loop.exe（Grounding Loop）随阶段 A 切片回归后重新提供（ADR-0010）
```

零外部依赖、参考模式位级确定性：同 seed 必须逐字节复现（性能模式按 ADR-0009 容差复现，数字必须标注模式与环境元数据）。没有 benchmark 数据不宣称性能。

## 工作流约束（硬规则）

1. 开发前先读相关代码、测试、`git status` 与设计文档；不基于假设动工。
2. 非平凡功能先需求澄清（`grill-with-docs`）；复杂功能先 `to-spec` → `to-tickets` 再实现。
3. 修 Bug 先建立能复现原始故障的测试（`diagnosing-bugs`），再修复。
4. 小步实现，及时跑相关测试，阶段点跑全量 `build.bat --test`。
5. 完成后审查实际 diff，对照规格查遗漏与超范围改动（`code-review`）。
6. 只报告实际执行过的检查及其结果；禁止声称未运行的测试已通过。
7. 跨 Agent/会话交接：通过规格文件、ADR、`handoff` 产物与明确文件路径传递上下文，不依赖聊天记录。
8. 多个 Agent 不得在无隔离机制下并发修改同一工作区。
9. 未经明确授权不执行危险 Git 操作（reset/force/rebase 历史），不自动推送远程。
10. 不将密钥、令牌、密码写入任何文档或交接材料。

## Agent skills

### Issue tracker

本仓库使用本地 Markdown 任务记录（`.scratch/<feature>/` 约定）。详见 `docs/agents/issue-tracker.md`。

### Triage labels

五个 canonical triage roles（`needs-triage` / `needs-info` / `ready-for-agent` / `ready-for-human` / `wontfix`）回答"该不该做、谁来做"；本仓库在其上扩展了**工程推进态**（`design-review` / `implementing` / `verifying` / `done`）回答"做到哪一步"。两者共用票据头部的同一个 `Status:` 行；**`design-review → implementing` 只能由 owner 批准触发**。详见 `docs/agents/triage-labels.md`。

### Domain docs

Single-context 布局：仓库根 `GLOSSARY.md` + `docs/adr/`。详见 `docs/agents/domain.md`。

### 技能使用映射（技能文件位于 `.agents/skills/`，**37 个全量**，来自 owner 定制仓库 `Vin535LFJ/mattpocock-skills-zh-CN-custom`（中文版全量 + 4 处上游回填）；可复现安装：`npx -y skills@1.7.1 add Vin535LFJ/mattpocock-skills-zh-CN-custom -s '*' -a codex --copy -y`。⚠️ **必须锁定 `skills@1.7.1`**——`skills@1.7.2` 依赖 Node≥22 的 `fs/promises.glob`，本机 Node 20.19 会直接崩溃）

| 阶段 | 技能 |
|---|---|
| 需求澄清（追问式访谈，沉淀 ADR/术语） | `grill-with-docs` |
| 术语与架构决策维护 | `domain-modeling` |
| 整理确认过的需求 → 规格说明 | `to-spec` |
| 规格 → 有依赖关系的小任务 | `to-tickets` |
| 按规格/任务实现 | `implement`（复杂核心逻辑配 `tdd`） |
| 测试驱动小步实现 | `tdd` |
| 系统化复现/定位/修复 Bug | `diagnosing-bugs` |
| 完成后按 Standards + Spec 两轴审查 | `code-review` |
| 开发复盘 | `retro` |
| 跨会话/跨 Agent 上下文交接 | `handoff` |
| 仓库初始化配置（已于 2026-09-28 运行；仅在切换 tracker 等时重跑） | `setup-matt-pocock-skills` |

## 语言与文档纪律

- 中文优先（架构文档术语保留英文原文）。
- `PLAN.md` 之外不新建计划类文档，除非代码先行验证或用户明确要求。
- 操作流程以 `docs/agents/workflow-manual.md` 为准；该文件只做导航与操作，权威内容仍在 `PLAN.md` / `CURRENT.md` / `docs/design/` / `docs/adr/`。
- 修改既有文档时合并而非覆盖；用户手写内容永远保留。
