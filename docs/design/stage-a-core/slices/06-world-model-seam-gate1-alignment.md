# L2 · 切片 06「世界模型接缝 + Gate-1 对齐报告」详细设计

> **设计版本**：v1 · **批准方式**：预授权（owner「直接把整个切片链实现」指令，2026-10-14；范围 = 切片 03–06） · **批准日期**：2026-10-14
> 状态：**已按预授权执行**。
> 上承：[L0 §3.2 时序图（编排顺序权威）](../../00-总体设计.md)；[L1 §4 签名 / §5 切片 06](../概要设计.md)；契约：⑨ Tick 日志（applied/rejected + 逐阶段延迟）；验收移交：适配器异常隔离（自切片 03）。

## 1. 程序描述

收口阶段 A：第二适配器家族（WorldModelAdapter，单文件自注册，镜像感知接缝）+ `LoopDriver` 端到端编排（真值推进→观察→感知/世界模型→裁决→账本→指标→TickLog）+ `apps/omniworld_loop` CLI + **Gate-1 对齐报告**（新实现三判据 vs `pre-rebuild` 基线并排）。

## 2. 数据结构

- `WorldModelAdapter { predict(Tick, const WorldState&) -> Proposal[]; name(); }`（注意：世界模型**看得到** canonical 状态——与感知接缝的本质区别；其便签走**同一**裁决路径）。
- 注册表镜像 perception（按名排序枚举；OBJECT 库；裸标识符宏约定——problem-log 004）。
- `TickLog { tick; applied; announced; rejected; StageLatency{render/perceive/adjudicate/metrics} }`——**墙钟延迟永不进任何哈希**（契约③：wall-clock 仅诊断）。
- `RunConfig { ticks; seed; width; height; footprint; perception_adapters[]; world_models[] }`（无默认值——调用方显式）。
- `RunReport { ticks_run; hz; seconds; metrics; world_hash; ledger_hash; ledger_entries; applied/announced/rejected; latency_sum; adapter_faults }`。
- 内建模型最小集：`zero_delta`（原地）与 `linear_extrapolation`（pos+vel），置信度固定 **0.5**（v0.1 as-built 事实的延续）。

## 3. 关键机制

- 编排顺序 = L0 §3.2 权威时序图；适配器全部经注册表解析（未知名字 = 调用方错误，fail-fast 抛 `invalid_argument`）。
- **异常隔离（切片 03 移交的验收）**：每个适配器调用包 try/catch ——任何适配器抛异常只计 `adapter_faults`、不中断 tick 推进；测试用"抛异常适配器"实证。
- 确定性：状态演算全由 seed 决定；时间戳只进 latency 字段（已被排除在哈希外）；`world_hash`/`ledger_hash` 为逐字节复现判据。
- **replay 模型显式延后**：票据原文"replay/linear/zero"最小集；录放管线（rollout 录制）是独立特性且非 Gate-1 判据——v1 实现 zero+linear，replay 延后（预授权偏差，向 owner 报告）。

## 4. 流程逻辑

按 L0 §3.2 时序图（DRV 编排：advance→render→perception→world_model→adjudicate→ledger→metrics；每 tick 生成 TickLog）。

## 5. 边界情形

| 情形 | 行为 |
|---|---|
| 未知适配器/模型名 | 抛 invalid_argument（fail-fast） |
| 适配器抛异常 | 计 fault，跳过其便签，tick 继续 |
| 0 ticks | 合法（报告空转） |
| 全尺寸参数非法（w/h≤0） | 由观察生成器既有守卫抛错 |

## 6. 测试计划

1. 模型注册表：两名齐、排序、未知→nullptr。
2. `linear_extrapolation` 精确外推（0.5+0.25=0.75 二进制精确）；`zero_delta` 原地；两者 conf==0.5、target=显式 id。
3. 端到端：50 tick 跑通；账本非空；三指标齐现；applied>0。
4. **确定性配对**：同配置双跑 → world_hash / ledger_hash / 指标 map 全等；异 seed → world_hash 变。
5. **异常隔离**：注册"抛异常适配器"，混入运行 → 完成不崩、faults≥1、其他适配器照常入账。
6. TickLog 守恒：applied+announced+rejected == 账本条目数。

## 7. 与票据验收对照

| 票据 AC | 落点 |
|---|---|
| 世界模型单文件接入、家族无关性 | 模型注册表 + 测试 1/2（replay 延后已记录） |
| loop 全管线 + TickLog | 测试 3/6 |
| **Gate-1 对齐报告** | `benchmarks/regression/2026-10-14-stage-a-gate1-alignment.md`（独立产出物） |
| 全量 ctest 绿 + 逐字节复现 | 测试 4 + 全量运行 |
| 适配器异常隔离（03 移交） | 测试 5 |
| 出口判据逐条自查 | 报告内逐条 + 未达标如实标注 |
