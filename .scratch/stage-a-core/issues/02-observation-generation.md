# 02: 观察生成

**What to build:** 从 WorldState 生成有界视图——任意请求分辨率的俯视 RGB + 帧元数据（backend/device/format/dims）+ 按实体三态可见性（看见/未见/不确定）。看不见的语义是"未见"，不是空也不是零（契约⑧）。

**Blocked by:** 01（真值心跳）

**Status:** done — 2026-10-14（TDD 红→绿完成；L2 由 owner 批准；证据见 ## Comments）

**流程：** L2 详细设计 → owner 批准 → TDD 实现 → problem-log → 走查。

- [x] render() 以**参数**接收分辨率——代码中不存在写死的分辨率常量（旧 224 bug 从设计上消灭的实证）
- [x] 可见性三态：视野内 = Seen；视野外 = Unseen；遮挡/边界 = Uncertain——未见 ≠ 空值 ≠ 零
- [x] FrameMeta 携带 backend/device/format/dims（参考模式如实标注 cpu/host/RGB8）
- [x] 同 seed → 观察序列逐字节一致；非默认分辨率测试通过
- [ ] git diff 只触碰 observation 模块

## Comments

- 2026-10-14 · **TDD 红相位**：桩实现（全背景帧、空可见性）下全量构建，`ow_slice02` 按预期失败——15 处断言失败（像素/可见性/元数据），`ow_slice01` + `runtime_probe_smoke` 保持绿（红只红在该红处）。
- 2026-10-14 · **绿相位（全量验证）**：`build.bat --test` → ctest **3/3 通过**（runtime_probe_smoke 0.43s / ow_slice01 0.17s / ow_slice02 0.02s）。实现提交：`3d68787`。
- 2026-10-14 · **走查**：`git status` 改动范围 = `src/observation/`（新增模块）+ `tests/ow_tests.cpp` + `CMakeLists.txt` + 本 L2/票据——与 L2 §6-6 声明的伴随清单一致，无超范围改动。
- 2026-10-14 · 新坑：无（一次成型；投影与边界数值全部选二进制精确值）。
