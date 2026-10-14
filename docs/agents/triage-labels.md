# Triage 标签

Skills 使用五个 canonical triage roles。这个文件把这些 roles 映射到此 repo issue tracker 中实际使用的 label 字符串。

本仓库 tracker = local markdown（`.scratch/<feature>/issues/`），triage 状态记录为每个 issue 文件顶部附近的 `Status:` 行；label 字符串即 `Status:` 行的取值。

| Label in mattpocock/skills | Label in our tracker | Meaning                                  |
| -------------------------- | -------------------- | ---------------------------------------- |
| `needs-triage`             | `needs-triage`       | Maintainer needs to evaluate this issue  |
| `needs-info`               | `needs-info`         | Waiting on reporter for more information |
| `ready-for-agent`          | `ready-for-agent`    | Fully specified, ready for an AFK agent  |
| `ready-for-human`          | `ready-for-human`    | Requires human implementation            |
| `wontfix`                  | `wontfix`            | Will not be actioned                     |

当某个 skill 提到 role（例如 "apply the AFK-ready triage label"）时，使用此表中对应的 label 字符串。

编辑右侧列，使其匹配你实际使用的 vocabulary。

## 工程推进态（本仓库扩展）

上表回答"这张票**该不该做、谁来做**"（triage）；下表回答"这张票**做到哪一步**"（推进态）。两者共用票据头部同一个 `Status:` 行——取值集合 = triage role ∪ 工程推进态。**不新建第二个 backlog**：`.scratch/<feature>/issues/NN-*.md` 是唯一任务载体。

| `Status:` 取值 | 含义 | 谁可以推进到该态 |
| --- | --- | --- |
| `needs-triage` / `needs-info` | 待评估 / 待补信息 | owner |
| `ready-for-agent` | 已充分规格化，可开工。**注意：可开工 ≠ 可写代码**，仍需该切片 L2 获批 | owner |
| `design-review` | L2 详细设计已产出，等 owner 批准 | AI 提交 → owner 批准 |
| `implementing` | L2 已批准，TDD 实现中 | AI |
| `verifying` | 实现完成，跑全量测试 / 逐字节复现 / 走查 | AI |
| `done` | 验收条件全部有证据，必要文档已同步 | AI 提交 → owner 确认 |
| `ready-for-human` | 需人工实现（AI 不得自行推进） | owner |
| `wontfix` | 明确不做（须写原因） | owner |

**硬规则（PLAN §九 规则 #6 的机制化）**：

1. `design-review → implementing` 这一跳**只能由 owner 批准触发**；任何 AI 不得自行把票推进到 `implementing`。
2. 票面勾选（验收条件）只允许在**有可复现证据**时勾选，并在 `## Comments` 里写下证据出处（命令 + 产物路径 / 提交号）。
3. `done` 不等于"代码写完"，等于"验收条件全部有证据 + 必要文档已同步"（详见 `workflow-manual.md` 的 Definition of Done）。
