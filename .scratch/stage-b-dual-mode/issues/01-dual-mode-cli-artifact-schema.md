# 01: 双模式 CLI + 工件 schema

**What to build:** `omniworld_loop --mode reference|performance`（默认 reference，未知值 fail-fast）；JSON 工件（run report）携带 `mode` 字段 + 完整环境元数据（OS/CPU/编译器/构建模式/commit），Markdown 报告层引用同一值（写入口单一）。参考模式行为**逐字节不变**——这是阶段 B 的常驻回归条件。

**Blocked by:** None（can start immediately）

**Status:** done — 2026-10-16（预授权执行；证据见 ## Comments）

**流程：** 第一步产出本切片 L2（`docs/design/stage-b-dual-mode/slices/01-dual-mode-cli-artifact-schema.md`）→ owner 批准 → TDD 实现 → problem-log → 走查。

- [x] `--mode reference|performance` 落地，默认 reference；未知模式立即报错不猜测
- [x] JSON 工件含 `mode` + 环境元数据；Markdown 报告引用同一值（值来自单一写入口，不重复采集）
- [x] 参考模式全量 ctest 绿 + 同 seed 双跑 `world_hash`/`ledger_hash` 逐位一致
- [x] performance 模式 v1 = 与 reference 同路径 + 如实标注（不得假装 GPU——L1 风险 R-B2）；差异机制随 B2/B3 进入
- [x] git diff 范围 = `apps/` + `loop` 工件写出面 + tests/CMake/文书（L1 §3 声明）

## Comments

- 2026-10-16 · **TDD**：slice07（6 组）一次编写即绿（仅修一处测试自身 argc 数错的失配）；全量 ctest **8/8 绿**（ow_slice07 0.03s）。
- 2026-10-16 · **参考模式零行为变化（关键证据）**：`--ticks 600 --seed 42` 参考模式运行 → `world_hash=574361e62eec92b9` / `ledger_hash=459ff85132cc103e`，与 2026-10-14 对齐报告记录的基线**逐位一致**。
- 2026-10-16 · **CLI 冒烟**：`--mode performance --out build/test-report.json` 产出含 `mode`/`environment`/`metrics`/`world_hash` 的 JSON（确定性：两次序列化字节相等，slice07 断言）。
- 2026-10-16 · **走查**：diff = `src/loop/driver.{hpp,cpp}` + `apps/omniworld_loop.cpp` + `tests/ow_tests.cpp` + `CMakeLists.txt`（新增 OW_GIT_COMMIT 注入）+ L2/票据/文书——与票据声明的"apps/ + loop 增补 + tests/CMake"一致，无超范围。
- 2026-10-16 · **元数据诚实性修正**：MSVC 下 `__cplusplus` 恒 199711L（无 `/Zc:__cplusplus`），改用 `_MSVC_LANG`——报告的标准字段不再误导。
- 2026-10-16 · 新坑：无。
