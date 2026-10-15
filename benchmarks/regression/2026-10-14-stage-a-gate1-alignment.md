# Regression: Stage-A Gate-1 Alignment — 重建引擎 vs pre-rebuild 基线（2026-10-14）

## Claim under test

白纸重建的引擎（ADR-0010，切片 01–06）重新挣得 pre-rebuild 引擎的三条 Gate-1 判据（1106.7 Hz / 适配器零核心改动 / ECE 判别），且每一项差异被逐条解释。（PLAN §六 阶段 A 出口；对齐基线见 ADR-0011 §5。）

## Environment（完整元数据）

| Item | Value |
|---|---|
| OS | Windows 11 build 26200 |
| CPU | Intel i9-14900HX (24C/32T), 31.8 GiB RAM |
| GPU | RTX 4060 Laptop 8 GiB（本轮未用——参考模式纯 CPU，ADR-0009） |
| Compiler | MSVC 19.29.30133 (VS2019, x64) |
| Build A（速率运行） | **Release**：CMake 3.20 + Ninja，`-DCMAKE_BUILD_TYPE=Release`（/O2），`/utf-8 /permissive- /W4` |
| Build B（对照） | Debug 等价（build.bat 默认，/Od /RTC1 /MDd） |
| Commit | 本报告随阶段 A 收口提交落库（切片列车 01–06；`.scratch/stage-a-core/`） |
| Command | `cmake -B build-rel -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build-rel --target omniworld_loop` 后 `build-rel\omniworld_loop.exe --ticks 600 --seed 42 [--adapters ...]` |

## Run parameters（镜像旧基线：seed=42, ticks=600）

8 个实体（常速运动学，v1 无墙），观察 64×64 RGB 正交俯视，footprint=0.02，关联容差 0.1。面板：A 默认（2 检测器 + 2 世界模型）/ B 仅 blob_detect / C 仅 blob_detect_overconfident。

## Results

**（a）闭环速率**

| 运行 | 墙钟 | 速率 |
|---|---|---|
| Release · 面板 A | 0.0158 s / 600 ticks | **38,094 Hz** |
| Release · 面板 B / C | 0.0125 / 0.0109 s | 48,167 / 54,855 Hz |
| Debug 对照 · 面板 A | 0.1256 s | 4,777 Hz |
| 旧基线（Release, 224², 12 实体, 3 检测器, 弹墙世界） | 0.542 s / 600 ticks | 1,106.7 Hz |

**条件差异解释**：帧 64² vs 224²（像素 ~12×）、8 vs 12 实体、无弹墙、无逐 tick 摘要/JSON 落盘。数字不可直接比（工作量不同）；判据一致：**≥30 Hz 达标，Release 余量 ~1270×**。

**（b）适配器零核心改动**

第二检测器与两个世界模型均以**单文件**新增（+ CMake 一行/个），核心零 per-model 代码；注册表查找是核心唯一接入分支点。git 证据：切片 03/06 收口记录。**PASS**（与旧基线同类证据形态）。

**（c）指标判别力（ECE）**

| 运行（Release, 600 ticks, seed 42） | 账本条数 | 聚合 ECE | divergence | identity_stability |
|---|---|---|---|---|
| A 默认面板 | 9,746 | 0.4941 | 0.0169 | 0.9883 |
| B 仅诚实检测器 | 9,673 | 0.4977 | 0.0170 | 0.9883 |
| C 仅夸大检测器 | 9,673 | 0.4963 | 0.0170 | 0.9883 |

**在环聚合 ECE 无法区分 B/C。原因已定位（problem-log 005）**：9746 条中世界模型占 9600 条（置信度固定 0.5、准确率≈1 → 每条约 0.5 的 |acc−conf|），把检测器 ~300 条的真实差异淹没。**指标按下述机制级证据判定**：

- **机制级判别（精确）**：`ow_slice05` 单测——诚实组（conf 0.875, acc 7/8）**ECE == 0.0**；注入组（同错法, conf 1.0）**ECE == 0.125**，全二进制精确。ECE 对"置信度 vs 准确率差"的响应是精确的。
- **在环面板缺口（如实标注）**：两个内建检测器的置信度都是 v1 占位公式（`0.70+0.02·面积` vs 常数 0.99），**没有任何一方是"已校准的正常模型"**——该席位按设计留给阶段 C 的真实模型（ONNX）。只有世界模型固定 0.5（v0.1 as-built 延续）。

**判据（c）裁定：⚠️ PARTIAL**——机制达标（精确单测），在环判别随阶段 C 真实模型关闭。配套两项修正排期见下（F1/F2）。

## 阶段 A 出口判定

| 判据 | 及格线 | 新实现 | 结果 |
|---|---|---|---|
| (a) 端到端帧率 | ≥30 Hz | 38,094 Hz（Release）| ✅ PASS（余量 ~1270×）|
| (b) 适配器接入核心零改动 | 单文件接入 | 3 个新参与者全单文件，注册表唯一分支 | ✅ PASS |
| (c) 指标判别力 | 判别正常 vs 注入错误置信度 | 机制精确；在环面板无已校准模型 | ⚠️ **PARTIAL（如实标注）** |

**Stage A verdict: 核心引擎重建完成（6/6 切片，ctest 7/7）；(a)(b) PASS，(c) PARTIAL（关闭路径已排期）。**

## 确定性证据（三重）

1. **同配置复跑**：面板 B 两次运行 → `world_hash=574361e62eec92b9`、`ledger_hash=70ed1958194a1cad` 逐位一致（墙钟不同：0.0125 vs 0.0105 s——时间不进哈希，契约③）。
2. **跨构建模式**：Debug 与 Release 同配置 → 状态哈希一致（4,777 vs 38,094 Hz，字节相同）。
3. **跨面板**：A/B/C 的 `world_hash` 相同（世界演化与参与者无关——正是单一作者设计的效果）；`ledger_hash` 随面板不同（账本如实反映参与者集合）。
4. 套件级：`ow_slice06` 确定性配对测试 + 全量 ctest **7/7**。

## 实现过程中的修正（诚实记录）

1. 自注册宏 token-paste 拒吃限定名 → 裸标识符约定（problem-log 004）。
2. 聚合 ECE 被高产低信息生产者淹没 → 在环判别受限（problem-log 005）。

## 偏差与范围边界（如实声明）

1. **replay 世界模型延后**：票据原文"replay/linear/zero"最小集；v1 实装 zero_delta + linear_extrapolation。录放管线（rollout 录制）是独立特性且非 Gate-1 判据，延后（预授权偏差，已向 owner 报告）。
2. **真值世界 v1 无墙**：实体数十 tick 内飘出观察窗口（常速直线）→ 在环观察后段枯竭。列为 F2，随阶段 B 修（场景约束或墙）。
3. **观察 64×64 为本次运行参数**：分辨率是参数（契约⑧），224 不是常量——这正是新设计与旧 224 硬编码的区别。
4. 检测器置信度为占位公式、世界模型置信度固定 0.5——(c) PARTIAL 的直接原因。

## Findings 与后续排期

| # | 发现 | 归属 |
|---|---|---|
| F1 | 聚合 ECE 掩盖生产者结构 → 需要 per-producer ECE（旧报告分表的教训） | 阶段 B（Metrics 增量；problem-log 005） |
| F2 | 无墙常速真值世界使观察窗口后段枯竭 → 对齐运行需有界场景（墙/回绕/场景配置） | 阶段 B |
| F3 | 已校准"正常模型"席位空缺 → 由阶段 C 真实模型（ONNX）关闭 (c) 的在环部分 | 阶段 C |

## 结论

Stage A（核心引擎重建）完成：契约 9+3 条语义全部有代码与测试落点；三判据 (a)(b) 达标且余量巨大、(c) 机制级达标/在环如实标 PARTIAL；确定性位级证据三重。下一步：**SOP-7 阶段收口**（retro + 面试素材蒸馏），随后进入**阶段 B 双模式框架**（含 F1/F2 修补）。
