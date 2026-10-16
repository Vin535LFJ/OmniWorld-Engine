# Research Note — ONNX Runtime / TensorRT / 真模型接入（2026-10-16 检索）

> Trust label: [Research → Architecture Decision]——本 note 是阶段 C L1 的证据基座（C1 交付物之一）。方法：官方文档 / 官方仓库 API / NuGet / HuggingFace API 一等来源；每条附 URL 与检索日期；未能核验者标 UNVERIFIED，不作为决策依据。

## 1. ONNX Runtime

| 事实 | 来源 |
|---|---|
| 最新稳定版 **1.31.0**（2026-10-09 发布），提供 Windows x64 GPU 直连 zip：`onnxruntime-win-x64-gpu_cuda12-1.31.0.zip` / `..._cuda13-...`；CPU 包 `onnxruntime-win-x64-1.31.0.zip` | GitHub Releases API（api.github.com/repos/microsoft/onnxruntime/releases/latest），2026-10-16 |
| CUDA EP 依赖：**CUDA 12.8+ 或 13.0+，且 cuDNN 9.x（`cudnn64_9.dll`）**—— cuDNN 8 与 9 互不兼容 | onnxruntime.ai/docs/execution-providers/CUDA-ExecutionProvider.html |
| NuGet 通道：`Microsoft.ML.OnnxRuntime.GPU`（含 CUDA EP 与 TensorRT EP，Windows/Linux），1.31.0 于 10/9 发布 | nuget.org/packages/Microsoft.ML.OnnxRuntime.GPU |
| C/C++ API：C API 薄封装；zip/tgz 与 NuGet 两种分发 | onnxruntime.ai/docs/get-started/with-cpp.html |
| 构建工具链文档并列两句：“VS2019 through the latest VS2022 are supported”与“默认 Generator 为 VS2022，其他版本不支持”（后者疑指特定构建配置）；预编译二进制的 toolset 版本**未文档化 → UNVERIFIED** | onnxruntime.ai/docs/build/inferencing.html |

**本机适配（关键）**：本机 CUDA toolkit 12.6.20（runtime_probe 探测，2026-10-16 state-check 尚绿）。ORT 1.31 的 CUDA EP 要求 CUDA 12.8+ / cuDNN 9.x——**与 12.6 存在版本线缺口**。决策路径（C3 首消费时执行，非现在猜）：① 优先确认本机 cuDNN 版本与可升级性；② 若维持 CUDA 12.6：选用与之匹配的 ORT 版本线（research 表仅覆盖 1.27–1.31，更早版本线需在 C3 追加核验）；③ 若升级 CUDA/cuDNN：直接取 1.31 + cuda12 包。**结论：ORT 版本不预先 pin 死，留 C3 首消费票按本机实际状态定版并回填本 note。**

## 2. TensorRT（Windows 近况）

| 事实 | 来源 |
|---|---|
| 最新版 **11.4.0**，官方安装方式含 Tar/Zip（Windows 仍列） | docs.nvidia.com/deeplearning/tensorrt/release-notes/（2026-10-16） |
| **Windows 10 支持 deprecated：2026-10 后 TensorRT 不再支持 Win10**（文档 snippet 级） | TensorRT release notes（搜索引擎索引 snippet；本机为 Win11 26200，不受影响） |
| ORT 的 TensorRT EP 在 Windows GPU 包内可用（`onnxruntime_providers_tensorrt.dll`） | nuget.org；microsoft/onnxruntime issue #32278 |
| **open 风险**：CUDA13 的 ORT 构建绑定 TensorRT 10 ABI（`nvinfer_10.dll`），与 TensorRT 11 不匹配（issue #32278，2026-10-13 仍 open） | 同上 |
| "NVIDIA 弃用独立 Windows 安装包"——**UNVERIFIED**（官方渠道未见；仅 Win10 支持终止为实证） | — |

**裁定**（与 L1 Round 1 #2 一致）：C 阶段 TRT EP **预留不实现**。CUDA EP 基线实测后再评估；若启用需先确认 ABI 匹配（issue #32278 状态）。

## 3. RT-DETRv2-S

| 事实 | 来源 |
|---|---|
| 官方仓库 **`lyuwenyu/RT-DETR`**（注意：非 `lyuwenyulove`，后者 404），**Apache-2.0**，活跃维护 | GitHub API，2026-10-16 |
| 官方提供 ONNX 导出脚本 `rtdetrv2_pytorch/tools/export_onnx.py`（需 PyTorch 环境） | 仓库 contents API |
| 规模：**Params 20M / FLOPs 60G / AP^val 48.1**（640 输入） | 官方 README model zoo |
| 官方权重为 safetensors（HF `PekingU/rtdetr_v2_r18vd`，20.2M 参数）；**官方未发布 ONNX 权重** | HuggingFace API |
| 社区 ONNX：`onnx-community/rtdetr_r18vd/onnx/model.onnx` = **82,572,357 B（≈78.7 MiB）**，fp16 41.4MB / int8 21.7MB 变体同仓 | HuggingFace API（tree） |

## 4. Depth-Anything-V2-Small

| 事实 | 来源 |
|---|---|
| 官方仓库 **`DepthAnything/Depth-Anything-V2`**（注意：非 `LiheYoung` 路径的旧引用），**Apache-2.0** | GitHub API，2026-10-16 |
| **修正（2026-10-16 实测）**：research 初稿引用的社区仓 `fabio-sim/Depth-Anything-ONNX` 经 HEAD 实测 **401（门槛仓）**；实际采用 **`onnx-community/depth-anything-v2-small`**（公开、`license:apache-2.0` 标签、109k 下载），`onnx/model.onnx` HEAD 实测 **99,060,839 B**；另有 fp16/int8/q4 量化变体同仓 | HF API tree + HEAD，2026-10-16 |
| **官方无 ONNX 导出/权重**；ONNX 仅在 Community Support（`fabio-sim/Depth-Anything-ONNX`） | 仓库 contents API + README |
| 许可细节：README 原文 "Depth-Anything-V2-Small model is under the Apache-2.0 license"（Base/Large/Giant 为 CC-BY-NC-4.0） | 官方 README |
| 规模：**24.8M 参数**；官方 `.pth` 99,218,434 B；自导 FP32 ONNX 预计 ≈99MB | HuggingFace API |

## 5. 本机运行约束（RTX 4060 Laptop 8GB）

- 显存：两模型 FP32 合计 ≈180MB，余量充足（640×640/518×518 输入）。
- **Windows TDR：WDDM 默认 GPU 任务超时 2 秒**——长时 kernel / TensorRT 引擎构建有 TDR 风险（learn.microsoft.com WDDM TDR 文档）。
- cuDNN 9 / CUDA 12.8+（ORT 1.31 线）与 本机 12.6.20 的版本线缺口见 §1。
- CC 8.9（Ada 家族；Laptop 行 UNVERIFIED，同族业界普遍 8.9）。

## 6. 模型获取决策（给 C3/C4 的输入）

- **RT-DETRv2-S**：取社区 ONNX `onnx-community/rtdetr_r18vd/onnx/model.onnx`（HEAD 实测 82,572,357 B，与 research 一致；权重 Apache-2.0；社区导出的 provenance 在 manifest 记录），或 Kaggle 免费环境按官方脚本自导（备选路径）。
- **DA-V2-Small**：官方无 ONNX → 社区 `onnx-community/depth-anything-v2-small/onnx/model.onnx`（HEAD 实测 99,060,839 B；Apache-2.0 标签；仅 Small 尺寸宽松许可）或 Kaggle 自导（备选）。
- **下载实测（2026-10-16）**：`node tools/fetch_models.mjs` 首跑（幂等 + SHA256 pin）——两文件均按上述 URL 实测可取；SHA256 首次下载时 pin 定于 `tools/fetch_models.manifest.json`。
- SHA256 于 fetch 首跑 `--record` pin 定；URL 失效时 manifest 启用备选路径。
