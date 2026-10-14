# 领域文档

Engineering skills 探索 codebase 时，应如何消费这个 repo 的 domain documentation。

## 布局：single-context

```
/
├── GLOSSARY.md          ← 本仓库唯一的术语表
├── PLAN.md              ← 定位/目标函数/里程碑路线图与 Gate 体系/操作规则
├── docs/adr/            ← 架构决策记录（0008 起；见其 README 索引）
├── docs/design/         ← 权威设计层：L0 总体设计（含冻结契约 §2）/ 各阶段 L1-L2 / problem-log
└── src/
```

## 探索前先读取这些

1. repo 根目录的 **`GLOSSARY.md`**——命名与讨论必须使用其中词汇。
2. **`docs/design/00-总体设计.md`**——权威设计（2026-10-12 起）：World Contract 契约语义（§2，白纸共创冻结；PLAN §七 是指向它的指针）、总体架构（§3）、阶段划分（§4）。
3. **`docs/adr/`**——读取与当前处理区域相关的 ADRs（0008 方向重开 / 0009 双模式 / 0010 白纸重建 / 0011 后视镜）。
4. 与方向、契约、范围相关的既有证据（按需）：
   - `docs/17-research/2026-09-21-direction-decision.md`（方向证据，203 行 + 来源）
   - `docs/17-research/2026-09-24-github-reuse-map.md`（组件选型与许可依据）
   - `docs/design/problem-log.md`（踩坑记录，兼面试素材）

如果这些文件不存在对应内容，**静默继续**；不要提前建议创建。`/domain-modeling` skill 会在 term 或 decision 实际被解决时增量更新它们。

## 使用 glossary 的词汇

当你的输出命名某个 domain concept 时（issue title、refactor proposal、test name），使用 `GLOSSARY.md` 中定义的 term，遵守各词条的 _Avoid_ 字段。

如果你需要的概念还不在 glossary 中，这是一个信号：要么你在发明项目没在使用的语言（重新考虑），要么存在真实缺口（为 `/domain-modeling` 记录）。

## 标记 ADR / 决策冲突

如果你的输出与现有决策矛盾，明确指出而不是静默覆盖：

> _Contradicts `docs/design/00-总体设计.md` §2 契约④⑤（真值唯一作者、信念账本只记不改），but worth reopening because…_

重大方向/契约/边界级变更需要新 ADR（编号接续 `docs/adr/` README 索引），格式见 `.agents/skills/domain-modeling/ADR-FORMAT.md`。
