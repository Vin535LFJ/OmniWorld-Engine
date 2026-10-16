# 01: 依赖隔离 + provider 框架（C1）

**What to build:** ONNX Runtime 作为可选外部依赖被"关起来"：CMake 选项 `OW_WITH_ONNXRUNTIME`（默认 OFF），默认构建保持零外部依赖即参考模式不变式；配合一篇研究 note（ORT 版本 / CUDA EP / TensorRT Windows 近况 / 两模型导出与许可 / 文件体积，一手来源）与 `tools/fetch_models`（幂等、SHA256 校验、许可记录）。

**Blocked by:** None（can start immediately）

**Status:** done — 2026-10-16（预授权执行；证据见 ## Comments）

**流程：** 预授权执行（owner「推送吧，然后继续开发」指令链，2026-10-16；授权范围 = 阶段 C 切片；偏离即停并回报）。L2 → TDD → 走查 → 收口。

- [x] research note 落 `docs/17-research/2026-10-16-onnx-runtime-integration.md`：ORT 版本 + Windows CUDA EP 支持（MSVC 2019 兼容性）、TensorRT Windows 分发近况、RT-DETRv2-S 与 DA-V2-Small 的 ONNX 导出路径 + 许可 + 体积（一手来源 URL）
- [x] `tools/fetch_models`：幂等下载 + SHA256 校验 + 许可记录（模型文件不入 git；校验和记录于研究 note）
- [x] CMake `OW_WITH_ONNXRUNTIME` 选项默认 OFF；默认构建不链接 ORT（零外部依赖不变式的构建级证明）
- [x] ORT 的包含路径/链接约定文档化（供 C3/C4 消费：ORT 仅链入性能路径，核心模块零 include）
- [x] `--ep cpu` CLI 参数（Round 2 #6；cuda 在 C3 生效，ORT 缺席时 fail-fast 报错）

## Comments

- 2026-10-16 · **TDD**：slice15（--ep 解析）一次即绿；全量 ctest **13/13 绿**（ow_slice15 0.03s）。
- 2026-10-16 · **零依赖不变式（构建级证明）**：`build/CMakeCache.txt` 确认 `OW_WITH_ONNXRUNTIME:BOOL=OFF`——默认构建不引 ORT；ON 时缺 OW_ONNXRUNTUNT_ROOT 将 FATAL_ERROR（fail-fast）。
- 2026-10-16 · **research note 关键事实（一手来源，2026-10-16 检索）**：ORT 1.31.0 CUDA EP 需 CUDA 12.8+ / cuDNN 9.x，**本机 CUDA 12.6.20 存在版本线缺口**——ORT 版本不预 pin，留 C3 首消费按本机实际定版并回填 note；TensorRT Windows 近况 = Win10 支持 2026-10 后终止 + TRT EP ABI 绑定 open issue → TRT EP 预留不实现（与 L1 Round 1 #2 一致）；RT-DETRv2-S 官方 Apache-2.0 无 ONNX 权重（社区导出 82.6MB 可对账）；DA-V2-Small 官方 Apache-2.0、无 ONNX 导出（自导 ≈99MB）。
- 2026-10-16 · **fetch_models 烟雾**：`--record` 在未 pin 且文件缺席时非零退出（fail-fast 生效）；models/ 已入 .gitignore。
- 2026-10-16 · **偏离记录**：一处拼写修正（OW_WITH_ONNXRUNTUNT 双 T，自查捕获）；无其他偏离。
