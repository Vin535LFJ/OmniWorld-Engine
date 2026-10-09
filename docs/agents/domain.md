# 领域文档

Engineering skills 探索 codebase 时，应如何消费这个 repo 的 domain documentation。

## 布局：single-context

```
/
├── GLOSSARY.md          ← 本仓库唯一的术语表
├── docs/adr/            ← 2026-09-28 起的架构决策记录（见其 README 索引）
├── PLAN.md              ← 项目级决策的唯一现行来源（World Contract v0.1、Gates、范围）
└── src/
```

## 探索前先读取这些

1. repo 根目录的 **`GLOSSARY.md`**——命名与讨论必须使用其中词汇。
2. **`docs/adr/`**——读取与当前处理区域相关的 ADRs。
3. 与方向、契约、范围相关的既有决策记录（在归档区，见 `docs/adr/README.md` 索引）：
   - `docs/17-research/2026-09-21-direction-decision.md`（方向证据，203 行 + 来源）
   - `docs/17-research/2026-09-24-github-reuse-map.md`（组件选型与许可依据）
   - `PLAN.md` 第 7 节（World Contract v0.1，冻结于 2026-09-28）

如果这些文件不存在对应内容，**静默继续**；不要提前建议创建。`/domain-modeling` skill 会在 term 或 decision 实际被解决时增量更新它们。

## 使用 glossary 的词汇

当你的输出命名某个 domain concept 时（issue title、refactor proposal、test name），使用 `GLOSSARY.md` 中定义的 term，遵守各词条的 _Avoid_ 字段。

如果你需要的概念还不在 glossary 中，这是一个信号：要么你在发明项目没在使用的语言（重新考虑），要么存在真实缺口（为 `/domain-modeling` 记录）。

## 标记 ADR / 决策冲突

如果你的输出与现有决策矛盾，明确指出而不是静默覆盖：

> _Contradicts PLAN.md §7 World Contract v0.1 (belief 层不可变更拥有字段), but worth reopening because…_

重大方向变更需要新 ADR（编号接续 `docs/adr/` README 索引），格式见 `.agents/skills/domain-modeling/ADR-FORMAT.md`。
