# 项目掌握指南 —— 学习 · 复盘 · 面试准备

> **这份文件是什么**：一条**学习路线**——告诉你在每个阶段该读什么、怎么读、读完应该能回答什么问题。
>
> **这份文件不是什么**：它不复制权威内容。方向看 [`PLAN.md`](../PLAN.md)，现在做什么看 [`CURRENT.md`](../CURRENT.md)，契约与架构看 [`docs/design/`](design/)，决策看 [`docs/adr/`](adr/)，操作流程看 [`docs/agents/workflow-manual.md`](agents/workflow-manual.md)。冲突时以权威文件为准。
>
> **生成日期**：2026-10-15 · 基于**实际运行验证**（非文档转述）。

---

## 0. 怎么用这份文档

三条路线，按你手上有的时间选：

| 你有的时间 | 走哪几节 | 目标 |
|---|---|---|
| **30 分钟速通** | §1 → §2.1 → §4.3 | 能说清"这是什么、跑起来长什么样" |
| **半天系统掌握** | §1 → §2 → §3 → §4 → §5 | 能对着代码讲清每一层 |
| **面试前冲刺** | §2.3 → §5.2 → §6 | 能接住追问、能讲出加分点 |

**一个总原则**：这个项目的价值不在"用了什么高级技术"，而在**每条设计决定都有理由、且每条主张都有可复现证据**。学习时始终问自己两句：*为什么这么设计？我怎么证明它真的做到了？*

---

## 1. 第一层：建立"这是什么"（30 分钟）

### 1.1 按这个顺序读

| 顺序 | 文件 | 只看什么 | 时间 |
|---|---|---|---|
| ① | [`README.md`](../README.md) 或 [`README.zh-CN.md`](../README.zh-CN.md) | 一句话定位 + 文档地图 | 3 min |
| ② | [`GLOSSARY.md`](../GLOSSARY.md) | **只扫标题**，混个脸熟：WorldState / Proposal / Adjudication / Divergence / ECE | 5 min |
| ③ | [`PLAN.md`](../PLAN.md) §一、§三、§四 | 三句话定位 / 终局画面 / 目标函数 | 10 min |
| ④ | [`CURRENT.md`](../CURRENT.md) | 现在做到哪了 | 2 min |
| ⑤ | [`docs/design/00-总体设计.md`](design/00-总体设计.md) §3 的三张 Mermaid 图 | **看图不看文字**——容器图 + 三个关键流程 | 10 min |

### 1.2 读完你应该能脱口而出

- 这是一个**世界引擎**，不是游戏引擎、不是大模型本体；评测只是它的第一个应用。
- 它要解决的问题：让**视频解码 / AI 推理 / 世界状态 / 渲染**在同一块 GPU 上以最少拷贝流动。
- 组织核心是一个 canonical 世界状态 + **World Contract**（identity / time / ownership / confidence）。
- 它当前的样子：一个纯 CPU、零依赖、能逐字节复现的最小闭环。

> ⚠️ **不要一开始就读 PLAN.md 全文**。它有 124 行、包含大量历史层积与已废止的旧方向，容易迷路。先读 §一/§三/§四，其余按需。

---

## 2. 第二层：技术链路

### 2.1 数据流：一个 tick 的旅程

这是整个项目的主干，**先把它背下来**，其余都是细节：

```
真值世界推进 → 观察生成 → 感知适配器 → 裁决 → 信念账本 → 指标
   WorldState    Observation    Proposal    Verdict   BeliefEntry  MetricReport
```

对应代码：`src/loop/driver.cpp` 的 `LoopDriver::run()` 里那个 `for (Tick t = 1; t <= cfg.ticks; ++t)` 循环。**这 90 行是整个引擎的心脏**，值得逐行读。

关键点：
- 每个 tick 重复一遍；默认 2000 遍。
- 参与者（适配器）**只能看、只能提议**，永远不能改真值。
- 墙钟时间只用于诊断，**不进任何哈希**（契约③）。

### 2.2 模块职责与调用关系

| 模块 | 目录 | 一句话职责 | 依赖谁 | 谁依赖它 |
|---|---|---|---|---|
| 公共值类型 | `src/core/types.hpp` | Tick / EntityId / Vec3 / Transform / 三态可见性 | — | 全部 |
| 便签契约 | `src/core/proposal.hpp` | Proposal 五元组 + 格式校验 | core/types | 感知、世界模型、裁决、账本 |
| 确定性随机 | `src/core/rng.hpp` | splitmix64：同 seed → 同序列 | — | 真值世界 |
| 确定性哈希 | `src/core/hash.hpp` | FNV-1a：给状态/账本算指纹 | — | 观察、编排、测试 |
| 世界状态 | `src/world_state/` | canonical 容器；**对外只读** | core/types | 全部 |
| 真值世界 | `src/truth_world/` | 确定性运动学；**WorldState 的唯一写者** | core/rng, world_state | 编排 |
| 观察生成 | `src/observation/` | 有界视图：RGB + 帧元数据 + 三态可见性 | world_state（只读） | 感知适配器、编排 |
| 感知接缝 | `src/perception/` | 接口 + 自注册表 + 内置检测器 | observation, proposal | 编排 |
| 裁决 | `src/adjudication/` | 可替换关联策略：Matched / NewEntity / Rejected | world_state（只读） | 编排 |
| 信念账本 | `src/belief/` | 追加式记录（便签 + 裁决结果） | adjudication | 指标、编排 |
| 指标 | `src/metrics/` | 深模块：divergence / ECE / 身份稳定性 | world_state, belief（只读） | 编排 |
| 世界模型接缝 | `src/world_model/` | 第二类适配器：预测下一状态 | world_state（只读） | 编排 |
| 编排 | `src/loop/` | **唯一知道所有模块的组件** | 以上全部 | CLI |
| CLI | `apps/omniworld_loop.cpp` | 命令行入口 + 打印报告 | loop | — |
| 测试 | `tests/ow_tests.cpp` | 6 个切片测试（按名字分派） | ow_core | — |

**依赖方向的铁律**：所有跨模块依赖**单向汇入 `loop`**，依赖图无环。看到任何一个模块去 include 别的模块时，问一句：*这个方向合理吗？*

### 2.3 三条设计主线（为什么代码长这样）

**① 单写者 —— 用编译器守规矩，而不是靠纪律**

`WorldState` 的公开面只有 4 个只读函数；改数据的 `mint()` / `step()` 是 `private`，只有 `friend` 的 `TruthWorld` 能调。这不是"我们约定别乱写"，而是**别人根本写不了**。感知适配器更彻底：它的头文件里**根本不 include `world_state`**，结构上够不着真值。

> 面试价值：这是"把架构约束下沉到类型系统"的典型手法，比写文档说"请不要直接改"强得多。

**② 端口与适配器 —— 新参与者 = 一个文件**

接入一个新模型，只需要在**自己的单文件**里写一个类 + 一行 `OW_REGISTER_PERCEPTION(...)`，核心代码零改动。验收方式很硬：`git diff` 只显示那一个文件新增。

> 面试价值：这是可扩展性的**可验证**定义——不是"设计得灵活"，而是"加东西时核心 diff 为零"。

**③ 确定性 —— 同 seed 逐字节复现**

四个保证：固定遍历顺序（按 id 序，不用哈希表迭代）、无 fast-math、自研 splitmix64 与 FNV-1a（不依赖平台随机/哈希实现）、墙钟不进哈希。

> 面试价值：确定性是**可复现性的地基**——没有它，"性能提升 30%"这种话无法证伪，也就不是工程结论。

---

## 3. 第三层：代码阅读顺序

**不要按目录字母顺序读。** 按下面 5 批，每批读完先回答"验收问题"再看下一批。

### 批 1 · 数据长什么样（约 15 分钟）

```
src/core/types.hpp        (21 行)   ← 全是 struct / using，最容易读
src/core/proposal.hpp     (38 行)   ← Proposal 五元组 + 校验函数
src/world_state/world_state.hpp (40 行)
src/world_state/world_state.cpp (35 行)
```

**看什么**：`WorldState` 的 public 区和 private 区各有什么。注意 `friend class TruthWorld;` 那一行。

**验收问题**：为什么 `step()` 和 `mint()` 不在 public 区？如果我想从外部改一个实体的位置，编译器会怎么拦我？

### 批 2 · 谁在写世界（约 15 分钟）

```
src/truth_world/truth_world.hpp (21 行)
src/truth_world/truth_world.cpp (29 行)
src/core/rng.hpp                (28 行)
```

**看什么**：`advance()` 里那个 `while (ws.tick() < target) ws.step();`；`step()` 里为什么按 id 顺序遍历。

**验收问题**：`advance(ws, 5)` 在 tick 已经是 10 的时候会发生什么？为什么这是"防御性忽略"而不是报错？

### 批 3 · 世界怎么被看见（约 30 分钟）

```
src/observation/observation.hpp (54 行)  ← 先读注释，特别是"no default-resolution constant"
src/observation/observation.cpp (101 行)
src/core/hash.hpp               (30 行)
```

**看什么**：`render()` 的三段——填背景色 → 逐实体画方块 → 记录可见性。特别注意 `world_to_pixel()` 的钳制逻辑。

**验收问题**：`Seen / Unseen / Uncertain` 三个状态分别在什么条件下产生？为什么"未见"不能简单地用 0 或空值表示？

### 批 4 · 接缝与参与者（约 40 分钟）

```
src/perception/adapter.hpp      (41 行)  ← 注意：它 include 了什么、没 include 什么
src/perception/registry.cpp     (32 行)
src/perception/blob_internals.hpp (86 行)
src/perception/blob_detect.cpp  (24 行)
src/perception/blob_detect_overconfident.cpp (37 行)
src/world_model/world_model.hpp (40 行)
src/world_model/linear_model.cpp (32 行)
```

**看什么**：`OW_REGISTER_PERCEPTION` 宏展开后是什么；`blob_components()` 的连通块搜索（用栈而不是递归，为什么？）；两个检测器的置信度公式为什么故意不同。

**验收问题**：新加一个检测器，需要改核心的哪几行？（答案：零行，只在 CMakeLists 加一行文件名。）

### 批 5 · 裁决、记账、算分、编排（约 60 分钟）

```
src/adjudication/adjudication.hpp (40 行)
src/adjudication/adjudication.cpp (40 行)  ← 三分支逻辑全在这
src/belief/ledger.hpp             (35 行)
src/metrics/metrics.hpp           (35 行)
src/metrics/metrics.cpp           (132 行) ← 三个指标的算法
src/loop/driver.hpp               (63 行)
src/loop/driver.cpp               (166 行) ← ★ 最重要，逐行读
apps/omniworld_loop.cpp           (82 行)
tests/ow_tests.cpp                (961 行) ← 最后读；先看 main() 的分派，再挑 slice05
```

**看什么**：`driver.cpp` 里那个 tick 循环——它是 §2.1 那张图的代码实现。以及 `hash_world()` / `hash_ledger()` 到底把什么算进了指纹。

**验收问题**：如果把观察分辨率从 64×64 改成 128×128，`world_hash` 会变吗？`ledger_hash` 呢？（提示：想清楚"哪些东西属于世界本身"。）

---

## 4. 第四层：能不能跑、怎么跑（已实测）

### 4.1 依赖与环境（实测清单）

| 项 | 要求 | 本机实测 |
|---|---|---|
| 操作系统 | Windows | Windows 11 build 26200 |
| 编译器 | MSVC（VS2019 及以上） | MSVC 19.29.30133 (VS2019, x64) |
| 构建系统 | CMake ≥ 3.20 + Ninja | CMake 3.20.0-rc1 |
| 语言标准 | C++20 | 已启用（但代码实际只用 C++11 级设施） |
| **引擎外部依赖** | **零** | ✅ `ow_core` 不链接任何第三方库 |
| CUDA Toolkit | **可选**（只有 `runtime_probe` 探测用） | `find_package(CUDAToolkit QUIET)`，找不到只警告 |
| GPU | **不需要**（参考模式纯 CPU） | RTX 4060 Laptop 8 GiB，本轮未使用 |

> **这是这个项目最值得说的工程事实之一：引擎本体零外部依赖。** 没有 ONNX、没有 Eigen、没有 Boost。所有基础件（随机数、哈希、容器）都是自研或标准库。

### 4.2 构建

```bash
# 方式一：一键（进入 MSVC 环境 + configure + build + 跑测试）
build.bat --test

# 方式二：Release 版（性能数字要用这个）
cmake -B build-rel -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-rel --target omniworld_loop
```

⚠️ `build.bat` 里写死了本机的 vcvarsall 路径（`D:\Program Files (x86)\Microsoft Visual Studio\2019\Community\...`）。**换机器要改这一行**——这是唯一的环境耦合点。

### 4.3 运行（2026-10-15 实测输出）

```bash
# 跑全部测试
./build/ow_tests.exe slice01   # → slice01: all tests passed
./build/ow_tests.exe slice02   # → ... slice06 全部 PASS（6/6）

# 跑闭环
./build/omniworld_loop.exe --ticks 600 --seed 42
```

实测结果（Debug）：

```
ticks=600 seconds=0.145953 hz=4110.9
world_hash=574361e62eec92b9 ledger_hash=459ff85132cc103e entries=9746
applied=9746 announced=0 rejected=0 faults=0
divergence=0.016946 ece=0.494074 identity_stability=0.988314
latency_ms: render=14.302 perceive=73.049 adjudicate=38.741 metrics=17.038
```

同一个命令用 Release 版跑：`hz=40873.6`，**两个哈希完全相同**。硬件探针：`./build/runtime_probe.exe --json` 正常输出 CPU/CUDA/Vulkan 探测结果。

### 4.4 自己验证确定性（面试时能现场演示）

```bash
./build/omniworld_loop.exe --ticks 600 --seed 42   # 记下 world_hash
./build/omniworld_loop.exe --ticks 600 --seed 42   # 再跑一次
./build/omniworld_loop.exe --ticks 600 --seed 43   # 换个 seed
```

实测：前两次 `world_hash` 都是 `574361e62eec92b9`，但墙钟时间不同（0.1459s vs 0.1432s）；换 seed 43 后变成 `dff70e1c772e452d`。

**这三行输出就是"确定性"的完整证明**：结果一致 + 时间不一致（时间不进哈希）+ 换输入结果变。

### 4.5 常见坑

| 现象 | 原因 | 处理 |
|---|---|---|
| `build.bat` 报找不到 vcvarsall | 路径写死，换机器了 | 改 `build.bat` 第 5 行 |
| Release 数字和报告对不上 | 运行间波动（报告 38,094 Hz，本次实测 40,874 Hz） | 属正常波动；**必须标注构建模式** |
| 找不到 `omniworld_loop.exe` | 它不在 `build/` 根下就是要先构建 | 先跑 `build.bat` |

---

## 5. 第五层：成果与完成度

### 5.1 阶段 A 交付清单（6/6 切片）

| 切片 | 交付物 | 状态 |
|---|---|---|
| 01 真值心跳 | 确定性运动学 + 单写者 WorldState | ✅ |
| 02 观察生成 | 有界视图 + 三态可见性 + 帧元数据 | ✅ |
| 03 感知接缝 | Proposal 五元组 + 自注册表 + 双检测器 | ✅ |
| 04 裁决与账本 | 可替换关联策略 + 追加式账本 | ✅ |
| 05 指标组 | divergence / ECE / 身份稳定性（可插拔） | ✅ |
| 06 世界模型 + 收口 | 第二接缝 + LoopDriver + CLI + Gate-1 报告 | ✅ |

**测试**：7/7（`runtime_probe_smoke` + `ow_slice01`–`06`），本次实测全绿。
**证据**：[`benchmarks/regression/2026-10-14-stage-a-gate1-alignment.md`](../benchmarks/regression/2026-10-14-stage-a-gate1-alignment.md)。

### 5.2 Gate-1 三判据判定（**注意这里有诚实标注**）

| 判据 | 及格线 | 结果 |
|---|---|---|
| (a) 端到端帧率 | ≥30 Hz | ✅ **PASS**，Release ~40,000 Hz（余量千倍级） |
| (b) 适配器零核心改动 | 单文件接入 | ✅ **PASS**，3 个新参与者全是单文件 |
| (c) 指标判别力 | 区分正常 vs 注入错误置信度 | ⚠️ **PARTIAL** |

**(c) 为什么只有 PARTIAL —— 这是最好的面试素材之一：**

单测里 ECE 能**精确**分辨（诚实组 `0.0`、夸大组 `0.125`）；但整个闭环跑起来后两者几乎一样（`0.4977` vs `0.4963`）。原因已定位（`problem-log 005`）：一轮 9746 条便签里，**世界模型贡献了 9600 条**、置信度固定 0.5，把检测器那 ~300 条的真实差异淹没了。

> **不是指标写错了，是"取平均"丢掉了来源结构。** 修法（按生产者分别算 ECE）已排进阶段 B。

### 5.3 已知缺口

| # | 缺口 | 归属 |
|---|---|---|
| F1 | 聚合 ECE 掩盖生产者结构 → 需 per-producer ECE | 阶段 B |
| F2 | 真值世界无墙，实体几十 tick 后飞出观察窗口 | 阶段 B |
| F3 | 缺"已校准的正常模型"席位 → 由真实模型关闭 (c) | 阶段 C |

---

## 6. 第六层：面试要点与可能的问题

### 6.1 先搭好你的"系统设计叙事"骨架

**90 秒版**（开场用）：

> 我在做一个 GPU 原生的世界引擎。核心想法是：让视频解码、AI 推理、世界状态、渲染共享**同一个世界状态**，而不是各自维护副本。围绕它我定义了一份契约——身份由 runtime 唯一铸造、时间用单调 tick、每个字段只有一个写者、所有非拥有者的写入都必须走"提议 + 裁决 + 记账"这条路。
> 目前完成了第一阶段：一个纯 CPU、零外部依赖、能逐字节复现的最小闭环，六个垂直切片，端到端 4 万 Hz。最有意思的收获是——我发现我的指标设计有个缺陷：聚合平均会被产量最大的生产者绑架，这个坑我写进了问题日志。

**5 分钟版**：在 90 秒版上补三件事——① 三条设计主线（单写者 / 端口适配器 / 确定性）；② Gate-1 三判据怎么定的、为什么 (c) 我标了 PARTIAL；③ 一个具体的踩坑故事（见 §6.4）。

### 6.2 高概率问题清单

**A. 架构与设计**

| 问题 | 答法要点 |
|---|---|
| 为什么每个字段只能有一个写者？ | "数据谁写坏的"永远可回答；冲突不需要仲裁（真值侧冲突集为空）。这是评测完整性的前提。 |
| 怎么保证"只有一个写者"？ | 不是靠约定——`WorldState` 的写接口是 private，只有 `friend TruthWorld` 能调。**编译期保证**。 |
| 为什么让适配器只能出 Proposal，不能直接写状态？ | ① 结构上杜绝污染真值；② 所有参与者的话都被记录下来，错误变成可测量的数据而不是事故。 |
| 新增一个模型要改多少代码？ | 零行核心代码。一个新文件 + 一行自注册宏 + CMake 一行。`git diff` 可证。 |
| 为什么要世界模型这条"第二接缝"？ | 证明契约是**家族无关**的——感知模型和世界模型是两类完全不同的参与者，走同一条路。 |

**B. 确定性与可复现性**

| 问题 | 答法要点 |
|---|---|
| 怎么做到逐字节复现？ | 固定遍历顺序（按 id 序，不用哈希表迭代）、无 fast-math、自研 splitmix64 与 FNV-1a、墙钟不进哈希。 |
| 为什么时间不进哈希？ | 时间只用于诊断。一旦进哈希，"同 seed 复现"就不可能——这是契约级的决定，写在契约③。 |
| 怎么证明它真的确定？ | 现场跑两遍：哈希相同、墙钟不同；再换构建模式（Debug/Release）跑，哈希依然相同。 |
| 性能数字为什么不能和旧基线直接比？ | 工作量不同（64² vs 224² 帧、8 vs 12 实体、无弹墙）。**数字必须带环境元数据与模式标注**，否则不是证据。 |

**C. 指标与评测**

| 问题 | 答法要点 |
|---|---|
| divergence / ECE / 身份稳定性分别测什么？ | 预测偏差 / 置信度校准 / 跨 tick 关联一致性。 |
| ECE 怎么算？ | 按置信度分 10 个桶，比较每桶"平均置信度"与"实际正确率"，差的绝对值加权求和。 |
| 为什么你的 (c) 判据只有 PARTIAL？ | **主动说**：聚合 ECE 被高产低信息的生产者淹没了（9600/9746 条来自世界模型）。指标本身没错，是聚合丢了结构。已排期按生产者分表。 |
| 置信度能不能影响裁决？ | 不能，契约明确"零裁决权力"——否则模型会无脑报 0.99 刷存在感。它唯一的消费者是校准指标。 |

**D. 工程实践**

| 问题 | 答法要点 |
|---|---|
| 为什么用 C++20 但代码里几乎看不到新特性？ | "boring core" 纪律：语言特性也要 usage-driven。而且我做过实验——把标准降到 C++17 一样能编过（`problem-log 003`）。 |
| 为什么用 OBJECT library 而不是 static library？ | MSVC 上静态库会丢掉自注册对象（没人引用的翻译单元被裁掉）。这是踩过的坑。 |
| 测试怎么写才算数？ | 行为断言 + 确定性断言**成对出现**。只写"每次结果一样"的测试，在空实现下会平凡通过（`problem-log 002`）。 |
| 没有 CI 怎么保证质量？ | 每次切片收口跑全量 `build.bat --test`；性能主张必须落 `benchmarks/regression/` 带环境元数据。 |

### 6.3 最容易被追问的 6 个深水区

准备好这些，面试官会觉得"这人是真的自己写的"：

1. **`friend` vs Writer 令牌**：为什么现在用 `friend` 而不是构造注入一个写令牌？→ 因为 v1 只有一个写者，`friend` 够用且最简；令牌是为"将来出现第二写者"准备的，那需要先走 ADR。**（展示"不为未来过度设计"的判断力）**
2. **连通块搜索为什么用栈不用递归**：避免深递归爆栈，且迭代顺序固定 → 确定性。
3. **`variant<EntityId, NewEntity>` 为什么不用 bool 标志**：让"第三种情况"在编译期就不可能出现。
4. **为什么 Observation 的可见性用 `pair<EntityId, Visibility>` 而不是平行数组**：消费者不该耦合到实体的迭代顺序。
5. **为什么分辨率必须是参数**：旧实现有个 224 硬编码的 bug；新设计让"写死分辨率"在结构上不可能（`RenderConfig` 没有默认值）。
6. **哈希函数为什么自己写**：标准库的 `std::hash` 在不同实现上结果不同 → 破坏跨机器复现。

### 6.4 三个"反直觉"的加分点（都在 problem-log 里）

这三个故事比任何技术名词都值钱——它们展示的是**判断力**，不是知识量：

| 故事 | 反直觉之处 | 一句话怎么讲 |
|---|---|---|
| `problem-log 002` | **空实现也能通过"确定性测试"** | 我写了"同 seed 两遍结果相同"的测试，结果实现还是空的时候它居然过了——两个空世界当然一样。教训：不变式断言必须配进展断言。 |
| `problem-log 003` | **兼容性争论用实验终结，不用观点** | 有人担心 C++20 太新。我把标准降到 17 重编一次，通过；恢复 20 再验一次，也通过。改一行、编一次、跑测试，比辩论快。 |
| `problem-log 005` | **聚合指标会被"产量最大"的分量绑架** | 平均 ECE 看起来正常，其实是被 9600 条低信息量的世界模型便签稀释了。任何"全体平均"型指标都要同时保留"按来源分解"。 |

---

## 7. 附录

### 7.1 文件地图（速查）

| 我想找… | 去哪 |
|---|---|
| 项目为什么存在 | `PLAN.md` §一/§三/§四 |
| 现在做到哪 | `CURRENT.md` |
| 契约的权威定义 | `docs/design/00-总体设计.md` §2 |
| 模块划分与接口签名 | `docs/design/stage-a-core/概要设计.md` |
| 某个切片的设计细节 | `docs/design/stage-a-core/slices/NN-*.md` |
| 踩过的坑 | `docs/design/problem-log.md` |
| 性能数字的出处 | `benchmarks/regression/` |
| 决策的来龙去脉 | `docs/adr/` |
| 术语的精确含义 | `GLOSSARY.md` |
| 我要开工 | `docs/agents/workflow-manual.md` |

### 7.2 学习时的三条纪律

1. **读到任何断言，先问"证据在哪"**。这个项目的规矩是：没有 benchmark 数据不宣称性能。
2. **读到任何设计，先问"不这么做会怎样"**。设计文档里每条模式都标了需求出处。
3. **不要相信这份文档，相信代码和实测**。本文所有数字都是 2026-10-15 实跑的；你自己再跑一遍，对不上就以下面的实际输出为准。
