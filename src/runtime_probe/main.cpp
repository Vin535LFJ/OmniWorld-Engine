#include "cpu_probe.hpp"
#include "cuda_probe.hpp"
#include "report.hpp"
#include "vulkan_probe.hpp"

#include <chrono>
#include <cstdio>
#include <cstring>

namespace {

using steady_clock = std::chrono::steady_clock;

double elapsed_ms(const steady_clock::time_point& from, const steady_clock::time_point& to) {
    return std::chrono::duration<double, std::milli>(to - from).count();
}

void print_usage(const char* program) {
    std::fprintf(stderr, "usage: %s [--text | --json]\n", program);
    std::fprintf(stderr, "exit codes: 0=ok 1=cpu_failed 2=cuda_failed 4=vulkan_failed (bitmask)\n");
}

}

int main(int argc, char** argv) {
    bool json_output = false;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--json") == 0) {
            json_output = true;
        } else if (std::strcmp(argv[i], "--text") == 0) {
            json_output = false;
        } else {
            print_usage(argv[0]);
            return 64;
        }
    }

    ow::section root{"runtime_probe"};
    auto& meta = root.add_child("meta");
    meta.add_field("timestamp", ow::now_iso8601_utc());
    meta.add_field("version", OW_VERSION_STR);
    meta.add_field("build_type", OW_BUILD_TYPE_STR);
    meta.add_field("cmake_version", OW_CMAKE_VERSION_STR);
    meta.add_field("compiler", ow::compiler_string());

    bool cpu_ok = false;
    bool cuda_ok = false;
    bool vulkan_ok = false;

    const auto total_start = steady_clock::now();

    const auto cpu_start = steady_clock::now();
    ow::section cpu = ow::probe_cpu(cpu_ok);
    const auto cpu_end = steady_clock::now();
    root.children.push_back(std::move(cpu));

    const auto cuda_start = steady_clock::now();
    ow::section cuda = ow::probe_cuda(cuda_ok);
    const auto cuda_end = steady_clock::now();
    root.children.push_back(std::move(cuda));

    const auto vulkan_start = steady_clock::now();
    ow::section vulkan = ow::probe_vulkan(vulkan_ok);
    const auto vulkan_end = steady_clock::now();
    root.children.push_back(std::move(vulkan));

    const auto total_end = steady_clock::now();

    auto& timing = root.add_child("timing");
    timing.add_field("cpu_probe_ms", ow::fmt_fixed(elapsed_ms(cpu_start, cpu_end), 3));
    timing.add_field("cuda_probe_ms", ow::fmt_fixed(elapsed_ms(cuda_start, cuda_end), 3));
    timing.add_field("vulkan_probe_ms", ow::fmt_fixed(elapsed_ms(vulkan_start, vulkan_end), 3));
    timing.add_field("total_ms", ow::fmt_fixed(elapsed_ms(total_start, total_end), 3));

    auto& result = root.add_child("result");
    result.add_field("cpu", cpu_ok ? "ok" : "failed");
    result.add_field("cuda", cuda_ok ? "ok" : "failed");
    result.add_field("vulkan", vulkan_ok ? "ok" : "failed");

    if (json_output) {
        ow::print_json(root);
    } else {
        ow::print_text(root);
    }
    std::fflush(stdout);

    int exit_code = 0;
    if (!cpu_ok) exit_code |= 1;
    if (!cuda_ok) exit_code |= 2;
    if (!vulkan_ok) exit_code |= 4;
    return exit_code;
}
