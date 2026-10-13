# 03: 感知接缝

**What to build:** PerceptionAdapter 接缝 + 单文件自注册机制 + 第一个内置检测器适配器入环——适配器只见观察、只出五元组便签；格式错误便签被拒收并留原因；单个适配器抛异常不阻断 tick 推进。

**Blocked by:** 02（观察生成）

**Status:** ready-for-agent

**流程：** L2 详细设计 → owner 批准 → TDD 实现 → problem-log → 走查。

- [ ] PerceptionAdapter 接口按 L1 §4；适配器拿不到 WorldState 引用（结构上只见 Observation）
- [ ] 内置检测器（blob 类）经 OW_REGISTER_* 在**自己的单文件**内自注册；核心零逐模型代码（git diff 实证——damped_linear 先例）
- [ ] Proposal 五元组齐全（producer/tick_observed/target/fields/confidence）；缺置信度等格式错误 → 拒收 + 原因留痕，不崩溃
- [ ] 适配器异常隔离测试：抛异常的 stub 适配器不阻断 tick 推进（L1 §6 建议级 → 本切片落地为验收）
- [ ] 同 seed → 便签序列逐字节一致
