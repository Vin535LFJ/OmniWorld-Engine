# Issue tracker：本地 Markdown

这个 repo 的 issues 和 specs 作为 markdown 文件存放在 `.scratch/` 中（仓库远程在 GitHub，但当前未使用 GitHub Issues；如需切换，编辑本文件并重跑 setup-matt-pocock-skills）。

## 约定

- 每个 feature 一个目录：`.scratch/<feature-slug>/`
- Spec 是 `.scratch/<feature-slug>/spec.md`
- Implementation issues 每个 ticket 一个文件，路径为 `.scratch/<feature-slug>/issues/<NN>-<slug>.md`，从 `01` 开始编号；绝不要写成一个 combined tickets file
- Triage state 记录为每个 issue file 顶部附近的 `Status:` 行
- Comments 和 conversation history 追加到文件底部的 `## Comments` heading 下

## 当 skill 说 "publish to the issue tracker" 时

在 `.scratch/<feature-slug>/` 下创建新文件（必要时创建目录）。

## 当 skill 说 "fetch the relevant ticket" 时

读取引用路径处的文件。用户通常会直接传入路径或 issue number。

## 与既有任务体系的关系

本仓库的路线图与阶段出口在 [PLAN.md](../../PLAN.md)（不放在 `.scratch/`）；`.scratch/` 只承载具体 feature 的 spec/tickets。`CURRENT.md` 指向"本周"，`.scratch/` 承载"这个 feature 的细节"。建议在仓库 `.gitignore` 外保留 `.scratch/`（当前未忽略——提交与否由 owner 决定）。
