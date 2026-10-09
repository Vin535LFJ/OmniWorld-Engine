# OmniWorld Engine

评测优先的 grounding-loop runtime：让感知模型、世界模型、仿真器、渲染器、智能体共享一个可测量的活世界状态。术语的中英文对照与完整定义见各词条；流程与边界见 [PLAN.md](PLAN.md)。

## Language

### 核心契约

**WorldState（世界状态）**:
runtime 拥有的 canonical、按 tick 版本化的世界表示；是唯一的真相层。
_Avoid_: 场景缓存、世界模型（指模型时）

**Entity（实体）**:
世界中一个带稳定身份的对象；由 runtime 铸造的 u64 id 标识，永不复用。
_Avoid_: 对象、actor

**Tick**:
runtime 拥有的单调时间步；一切因果以 tick 计，wall-clock 仅作诊断。
_Avoid_: 帧时间、时间戳（指 tick 域时）

**Proposal（提议）**:
非拥有者组件的唯一写路径：带置信度与观察 tick 的状态断言，由 runtime 裁决。
_Avoid_: 检测结果直写、直接更新

**Belief layer（信念层）**:
被采纳的 Proposal 的记录；只被评分，永不覆盖拥有字段。
_Avoid_: 副本状态、影子状态

**Ownership（所有权）**:
每个字段类只有一个拥有者（v0：真值世界独占运动学）；冲突策略见 PLAN.md。
_Avoid_: 写权限混称

**Observation（观察）**:
某 tick 的有界视图（v0：224×224 正交俯视 RGB + 九字段内存契约元数据）。
_Avoid_: 截图、渲染帧（指契约结构时）

**Adjudication（裁决）**:
runtime 对 Proposal 的身份关联与采纳/拒绝判定（v0：最近变换 + 类别匹配）。
_Avoid_: 匹配、过滤（指契约语义时）

### 评测与闭环

**Grounding Loop**:
被测量的闭环：真值推进 → Observation → 感知适配器 → 裁决 → 指标。
_Avoid_: 主循环、pipeline（指此闭环时）

**Truth World（真值世界）**:
种子确定性的运动学世界，是所有 belief 的评分基准。
_Avoid_: 模拟器（指内部真值时）、GT（口语可用，文档用全称）

**Divergence（发散度）**:
预测位置与实际下一状态的距离——Phase 1 的核心指标。
_Avoid_: 误差率、损失

**ECE（期望校准误差）**:
分箱比较置信度与实际正确率；双向抓失准的 Gate 1c 指标。
_Avoid_: 置信度错误率

**Gate**:
带预定义数值判据的阶段决策点（Gate 1 纯技术；Gate 2/3 双轨）。
_Avoid_: 里程碑（无判据的进度点不是 Gate）

### 参与者

**Adapter（适配器）**:
外部能力接入的唯一形态：单文件自注册（OW_REGISTER_* 宏），核心零分支。
_Avoid_: 插件、provider（指接入形态时）

**Perception Adapter / World Model Adapter**:
两类适配器家族：前者只吃 RGB 出 Proposal；后者按当前状态预测下一状态。
_Avoid_: 模型（不指明家族时）

**Rollout（回放数据）**:
真值世界的逐 tick 实体状态记录；replay 模型的数据、MLP 的训练集。
_Avoid_: 日志（指数据集时）

### 治理

**Track A / Track B**:
双轨成功模型——A 生态采用；B 个人技能资产与敲门砖工件。
_Avoid_: 主线/副线

**Regression Report（回归报告）**:
性能/指标主张的唯一证据形态：完整环境元数据 + 可复现命令 + 产物路径。
_Avoid_: 测试笔记（指基准证据时）
