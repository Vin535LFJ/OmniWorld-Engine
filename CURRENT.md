# CURRENT — 本周唯一焦点

> 只回答一个问题：**这周写什么代码。** 背景与全貌见 [`PLAN.md`](PLAN.md)。

**本周 = Week 2（Grounding Loop v0 第 2 周）**

## 本周交付（就这四件）

1. `src/world_state`：实体注册表（runtime 铸 ID）、tick 时钟域、状态版本号、transition 日志——字段照 [`PLAN.md`](PLAN.md) 第七节（World Contract v0），一个不多一个不少。
2. 确定性真值世界：N 个刚体、定种子、运动学推进。
3. 离屏 Observation 生成器（~200 行 CPU 光栅，224×224，画面丑无所谓）。
4. 确定性测试：同种子两遍，状态与观测序列逐字节一致（进 `ctest`）。

## 出口条件

- 确定性测试在 CI 通过；
- Observation 产出 ≥60Hz（实测数字记进 benchmark note）。

## 不碰

- 不接任何模型（Week 3 的事）、不装 TensorRT、不碰 Vulkan 渲染、不写文档（除非代码需要）、想法一律去 `docs/17-research/research-queue.md`。

完成后说"开始 Week 3"。
