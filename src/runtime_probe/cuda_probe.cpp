#include "cuda_probe.hpp"

#ifdef OW_HAS_CUDA
#include <cuda_runtime.h>
#endif

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace ow {

#ifdef OW_HAS_CUDA

namespace {

double median_of(std::vector<double> values) {
    if (values.empty()) {
        return 0.0;
    }
    std::sort(values.begin(), values.end());
    const size_t mid = values.size() / 2;
    if (values.size() % 2 == 1) {
        return values[mid];
    }
    return (values[mid - 1] + values[mid]) / 2.0;
}

std::string cuda_version_string(int v) {
    return std::to_string(v / 1000) + "." + std::to_string((v % 1000) / 10);
}

bool measure_device_bandwidth(size_t bytes, int samples, section& d) {
    unsigned char* device_buffer = nullptr;
    void* host_buffer = nullptr;
    cudaEvent_t start = nullptr;
    cudaEvent_t stop = nullptr;

    if (cudaMalloc(&device_buffer, bytes) != cudaSuccess) {
        return false;
    }
    if (cudaMallocHost(&host_buffer, bytes) != cudaSuccess) {
        cudaFree(device_buffer);
        return false;
    }
    std::memset(host_buffer, 0xA5, bytes);

    if (cudaEventCreate(&start) != cudaSuccess || cudaEventCreate(&stop) != cudaSuccess) {
        cudaEventDestroy(start);
        cudaEventDestroy(stop);
        cudaFreeHost(host_buffer);
        cudaFree(device_buffer);
        return false;
    }

    cudaMemcpy(device_buffer, host_buffer, bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(host_buffer, device_buffer, bytes, cudaMemcpyDeviceToHost);

    std::vector<double> h2d_ms;
    std::vector<double> d2h_ms;
    float elapsed = 0.0f;
    for (int i = 0; i < samples; ++i) {
        cudaEventRecord(start);
        cudaMemcpy(device_buffer, host_buffer, bytes, cudaMemcpyHostToDevice);
        cudaEventRecord(stop);
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&elapsed, start, stop);
        h2d_ms.push_back(static_cast<double>(elapsed));
    }
    for (int i = 0; i < samples; ++i) {
        cudaEventRecord(start);
        cudaMemcpy(host_buffer, device_buffer, bytes, cudaMemcpyDeviceToHost);
        cudaEventRecord(stop);
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&elapsed, start, stop);
        d2h_ms.push_back(static_cast<double>(elapsed));
    }

    const double p50_h2d = median_of(h2d_ms);
    const double p50_d2h = median_of(d2h_ms);
    const double best_h2d = *std::min_element(h2d_ms.begin(), h2d_ms.end());
    const double best_d2h = *std::min_element(d2h_ms.begin(), d2h_ms.end());
    const auto gbps = [bytes](double ms) {
        return static_cast<double>(bytes) / (ms * 0.001) / 1e9;
    };

    d.add_field("bandwidth_bytes", std::to_string(bytes));
    d.add_field("bandwidth_samples", std::to_string(samples));
    d.add_field("h2d_p50_ms", fmt_fixed(p50_h2d, 3));
    d.add_field("h2d_p50_gbps", fmt_fixed(gbps(p50_h2d), 2));
    d.add_field("h2d_best_gbps", fmt_fixed(gbps(best_h2d), 2));
    d.add_field("d2h_p50_ms", fmt_fixed(p50_d2h, 3));
    d.add_field("d2h_p50_gbps", fmt_fixed(gbps(p50_d2h), 2));
    d.add_field("d2h_best_gbps", fmt_fixed(gbps(best_d2h), 2));

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    cudaFreeHost(host_buffer);
    cudaFree(device_buffer);
    return true;
}

}

section probe_cuda(bool& ok) {
    ok = false;
    section s{"cuda"};

    int device_count = 0;
    cudaError_t err = cudaGetDeviceCount(&device_count);
    if (err != cudaSuccess) {
        s.add_field("status", "failed");
        s.add_field("error", cudaGetErrorString(err));
        cudaGetLastError();
        return s;
    }
    s.add_field("status", "ok");
    s.add_field("device_count", std::to_string(device_count));
    if (device_count == 0) {
        return s;
    }
    ok = true;

    int driver = 0;
    int runtime = 0;
    cudaDriverGetVersion(&driver);
    cudaRuntimeGetVersion(&runtime);
    s.add_field("driver_version", cuda_version_string(driver));
    s.add_field("runtime_version", cuda_version_string(runtime));

    for (int device = 0; device < device_count; ++device) {
        auto& d = s.add_child("device_" + std::to_string(device));
        d.add_field("index", std::to_string(device));

        const cudaError_t set_err = cudaSetDevice(device);
        cudaDeviceProp prop{};
        const cudaError_t prop_err = cudaGetDeviceProperties(&prop, device);
        if (set_err != cudaSuccess || prop_err != cudaSuccess) {
            d.add_field("status", "failed");
            d.add_field("error",
                        cudaGetErrorString(prop_err != cudaSuccess ? prop_err : set_err));
            continue;
        }
        d.add_field("status", "ok");
        d.add_field("name", prop.name);
        d.add_field("compute_capability", std::to_string(prop.major) + "." + std::to_string(prop.minor));
        d.add_field("sm_count", std::to_string(prop.multiProcessorCount));
        d.add_field("core_clock_mhz", std::to_string(prop.clockRate / 1000));
        d.add_field("memory_clock_mhz", std::to_string(prop.memoryClockRate / 1000));
        d.add_field("memory_bus_width_bits", std::to_string(prop.memoryBusWidth));
        d.add_field("l2_cache", fmt_mib(static_cast<uint64_t>(prop.l2CacheSize)));
        d.add_field("total_global_memory", fmt_gib(prop.totalGlobalMem));
        d.add_field("ecc", prop.ECCEnabled ? "enabled" : "disabled");
        d.add_field("unified_addressing", prop.unifiedAddressing ? "true" : "false");
        d.add_field("async_engine_count", std::to_string(prop.asyncEngineCount));
        d.add_field("pci",
                    std::to_string(prop.pciDomainID) + ":" + std::to_string(prop.pciBusID) +
                        ":" + std::to_string(prop.pciDeviceID));

        size_t free_bytes = 0;
        size_t total_bytes = 0;
        if (cudaMemGetInfo(&free_bytes, &total_bytes) == cudaSuccess) {
            d.add_field("free_global_memory", fmt_gib(free_bytes));
            d.add_field("used_global_memory", fmt_gib(total_bytes - free_bytes));
        }

        if (!measure_device_bandwidth(size_t{64} << 20, 5, d)) {
            d.add_field("bandwidth", "failed");
            cudaGetLastError();
        }
    }
    return s;
}

#else

section probe_cuda(bool& ok) {
    ok = false;
    section s{"cuda"};
    s.add_field("status", "unavailable");
    s.add_field("error", "built without CUDA toolkit (OW_HAS_CUDA undefined)");
    return s;
}

#endif

}
