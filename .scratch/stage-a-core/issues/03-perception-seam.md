# 03: 感知接缝

**What to build:** PerceptionAdapter 接缝 + 单文件自注册机制 + 第一个内置检测器适配器入环——适配器只见观察、只出五元组便签；格式错误便签被拒收并留原因；单个适配器抛异常不阻断 tick 推进。

**Blocked by:** 02（观察生成）

**Status:** done — 2026-10-14（预授权执行；证据见 ## Comments；AC「适配器异常隔离」按 L2 裁定移交切片 06——本切片无 loop 宿主）

**流程：** L2 详细设计 → owner 批准 → TDD 实现 → problem-log → 走查。

- [x] PerceptionAdapter 接口按 L1 §4；适配器拿不到 WorldState 引用（结构上只见 Observation）
- [x] 内置检测器（blob 类）经 OW_REGISTER_* 在**自己的单文件**内自注册；核心零逐模型代码（git diff 实证——damped_linear 先例）
- [x] Proposal 五元组齐全（producer/tick_observed/target/fields/confidence）；缺置信度等格式错误 → 拒收 + 原因留痕，不崩溃
- [x] 适配器异常隔离（stub 适配器不阻断 tick 推进）——**移交切片 06 落实并验证**（L2 §7 记录在案；本切片无 loop 宿主）
- [x] 同 seed → 便签序列逐字节一致

## Comments

- 2026-10-14 · **TDD 红相位（含真实踩坑）**：首跑编译失败——自注册宏对带限定名的类型做 token-paste（`make_ow`）+ 测试端漏 include 接缝头；修复后桩实现跑出断言红。教训已入 problem-log 004。
- 2026-10-14 · **绿相位（全量验证）**：`build.bat --test` → ctest **4/4 通过**（probe 1.53s / slice01 1.09s / slice02 0.02s / slice03 0.02s）。
- 2026-10-14 · **走查（diff 范围）**：`src/core/proposal.hpp`（新增）+ `src/perception/`（adapter/registry/blob_internals/blob_detect/blob_detect_overconfident）+ `observation.hpp` 提升背景哨兵常量（L2 已记录）+ tests/CMake/票据/docs——与 L2 §6-7 声明一致；第二适配器 = **恰好一个新文件 + 一行 CMake**（适配器不变式的首证）。
- 2026-10-14 · **裁决入档**：感知适配器不臆造类别（class_tag 对参与者永远可缺省；归并由裁决按位置匹配）——切片 05 联表评分不受影响。
