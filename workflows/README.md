# workflows/ — 工作流规格

> 每个文件 = **一个 loop 的规格**。loop 是"重复发生的活动"，workflow 是它的规格；你跑 workflow，loop 是它的运行实例。
> **工具无关**：任何 AI（ZCode / Codex / WorkBuddy）拿到文件即可执行。
> 素材与术语在 [`NOTES.md`](../NOTES.md)；操作手册在 [`docs/agents/workflow-manual.md`](../docs/agents/workflow-manual.md)。

## 索引

| # | Workflow | 触发 | 检查点 | 状态 |
|---|---|---|---|---|
| [W00](00-state-check.md) | **状态核验** | 事件（需要知道状态时） | 无 | ✅ 规格完成，脚本已实测 |
| [W01](01-handoff.md) | **跨 AI 交接** | 手动 + **换 AI 时**（由接手方自检触发） | 1（最后） | ✅ 规格完成 |
| [W02](02-stage-close-out.md) | **阶段收口** | 事件（最后一片 done） | 2 | ✅ 规格完成 |
| [W03](03-slice-development.md) | **切片开发** | 事件（CURRENT 指向票据） | 2（含硬门禁） | ✅ 薄规格 + 指针 |

## 衔接

```
W03 切片开发 ──最后一片 done──▶ W02 阶段收口 ──▶ 下一阶段
      │                                │
      └────────── 任一步都可能触发 W01 交接 ──────┘
                        │
                        └──▶ 第一步总是 W00 状态核验
```

## 未规格化但已识别的 loop（候选，待 grilling）

- **面试素材沉淀**——已并入 W02 步骤 4（阶段收口时批量蒸馏 → `docs/interview-narrative.md`）。
- **研究队列巡检**——`docs/17-research/research-queue.md` 的触发条件到期时该做什么，目前无流程。
- **检查接线**——`build.bat --test` 可用但未接入任何 hook/CI（retro 候选，尚未决定归属）。

## 维护纪律

- 本目录是**规格层**，不是权威层：不定义契约、目标函数或 Gate 判据，那些在 `PLAN.md` / `docs/design/`。
- 新增 workflow 前先找已有的——**不要为同一活动建第二份规格**。
- 规格的完成标准：**执行者读完无需再问任何问题**。
