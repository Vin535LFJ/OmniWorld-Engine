---
name: implement
description: "基于 spec 或 ticket 集合实现一段工作。"
disable-model-invocation: true
---

实现用户在 spec 或 tickets 中描述的工作。

如果用户传入了 ticket 引用，先从 issue tracker 取回它，并在开始前说明它的标题。如果引用有歧义，先询问。

尽可能在预先约定好的 seams 上，调用 Skill 工具执行 `tdd`。

定期运行 typechecking，定期运行单个测试文件，并在最后运行完整测试套件。

完成后，调用 Skill 工具执行 `code-review` 审查这次工作。

把工作提交到当前 branch。

<!-- 本地回填：同步上游 mattpocock/skills 的 implement 修复（ticket 标题前置 + 改用 Skill 工具调用 tdd/code-review，替换 /tdd 斜杠式）。详见 BACKPORTS.md -->
