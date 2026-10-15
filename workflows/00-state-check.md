# W00 · 状态核验（state-check）

> **用途**：用一条命令回答「这个仓库现在是什么状态」。
> **它是被调用方**：W01 交接、W02 收口、日常开工都先跑它，避免各自重复实现。
> **规格工具无关**——任何 AI（ZCode / Codex / WorkBuddy）拿到本文件即可执行。

---

## Trigger

**事件触发**：任何一次「我需要知道现在什么状态」的时刻——
- 接手工作时（W01 交接的第一步）
- 阶段收口前（W02 的第一步）
- 提交前 / 推送前
- 怀疑声明与实际不符时（交接声明曾两次失准：说"16 个提交"实为 17；说"工作树干净"实为有未跟踪文件）

不需要定时跑。

## Checkpoint

**无。** 这一步只读，不改变任何东西，不需要人确认。

## 输入

无。必须能在干净 clone 上直接跑（若 `build/` 未配置，报出该事实而不是崩溃）。

## 步骤

### 1. 跑权威命令（这是 source of truth）

```bash
bash scripts/state-check.sh
```

**完成判据**：脚本退出码 0 = 状态干净；非 0 = 有需要处理的事项，且脚本已逐条列出。

### 2. 脚本必须报告的五项

| # | 项 | 命令 | 为什么 |
|---|---|---|---|
| 1 | 分支与**待推送提交数** | `git status -sb` + `git rev-list --count origin/master..HEAD` | 交接声明与实际不符的高发区 |
| 2 | **工作树**（改动 + 未跟踪） | `git status --porcelain` | **未跟踪文件 = 可能未落盘的产出**（C1–C6 丢失的同一类风险） |
| 3 | **测试** | `ctest --test-dir build` | 项目核心主张的可执行证据 |
| 4 | **护栏是否真的在跑** | 比较 `tools/hooks/pre-commit` 与 `.git/hooks/pre-commit` | `.git/hooks/` **不受版本控制**：新 clone 拿到源文件却没有装好的 hook，门禁会**静默失效** |
| 5 | **文档链接** | 见下 | 死链曾因交接遗漏而产生并提交（commit `1d4f97a`） |

### 3. 链接检查的精确语义（这是本规格最容易做错的地方）

检查范围：**转向文件 + 设计文档**，即
`./*.md`、`docs/agents/*.md`、`docs/adr/*.md`、`workflows/*.md`、`docs/design/**/*.md`。
**排除** `.agents/`（vendor 技能内容，含示例路径）与 `docs/99-archive/`（历史，已知失效）。

三种失败，必须区分：

| 判据 | 含义 |
|---|---|
| `MISSING` | 目标在磁盘上不存在 |
| `UNTRACKED TARGET` | 目标存在但**未被 git 跟踪**——本地看着对，clone 下来是死链 |
| `UNTRACKED DIR` | 目录下没有任何被跟踪的文件 |

**实现纪律（性能，实测得出）**：Git Bash 每次 fork ≈ 0.3 s。**禁止**在逐链接循环里调用子进程
（`dirname`、`$(...)`、`git`、`grep` 都算）。正确做法：`git ls-files` 与 `grep -oH` **各调用一次**，
其余全部在内存中完成（关联数组 + 纯 bash 路径归一化）。
反面教材：per-link 调 `git ls-files --error-unmatch` → 48 s；改成内存后 → 9 s。

**另一个必踩的坑**：链接里的 `..` **必须归一化**后再与 `git ls-files` 比对
（`docs/agents/../../PLAN.md` ≠ `PLAN.md`，会误报）。且 `git ls-files` 默认对非 ASCII 路径做
C 风格转义，必须加 `-c core.quotePath=false`。

## 输出

stdout 报告 + **退出码**（0 = OK，非 0 = 需处理）。退出码是给下游 workflow 用的门禁信号。

## 完成判据（Definition of Done）

- [ ] `bash scripts/state-check.sh` 能跑完并给出退出码
- [ ] 四项（分支/工作树/测试/链接）都有输出
- [ ] 退出码与输出一致（有 `!!` 行就必须非 0）
- [ ] 在干净 clone（无 `build/`）上不崩溃，而是提示先构建
- [ ] 单次运行 < 15 s

## 已知限制（如实声明）

- 链接检查**只覆盖项目自己的转向与设计文档**；`knowledge/`、`docs/17-research/`、`.agents/` 不查。
  扩大范围会显著变慢，收益不明——需要时再单独开一条。
- 不检查**语义**（"这段描述还成立吗"），只检查**存在性与被跟踪性**。语义漂移仍需人读。
