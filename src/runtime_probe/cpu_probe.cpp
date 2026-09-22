#include "cpu_probe.hpp"

#include <bitset>
#include <cstdint>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <intrin.h>
#else
#include <fstream>
#include <sys/utsname.h>
#include <unistd.h>
#endif

namespace ow {

namespace {

std::string trim_spaces(std::string s) {
    const char* ws = " \t\r\n";
    const auto begin = s.find_first_not_of(ws);
    if (begin == std::string::npos) {
        return "";
    }
    const auto end = s.find_last_not_of(ws);
    return s.substr(begin, end - begin + 1);
}

std::string strip_quotes(std::string s) {
    if (!s.empty() && s.front() == '"') {
        s.erase(0, 1);
    }
    if (!s.empty() && s.back() == '"') {
        s.pop_back();
    }
    return s;
}

#ifdef _WIN32

std::string cpuid_vendor() {
    int regs[4] = {0, 0, 0, 0};
    __cpuid(regs, 0);
    char vendor[13];
    std::memcpy(vendor, regs + 1, 4);
    std::memcpy(vendor + 4, regs + 3, 4);
    std::memcpy(vendor + 8, regs + 2, 4);
    vendor[12] = '\0';
    return vendor;
}

std::string cpuid_brand() {
    int query[4] = {0, 0, 0, 0};
    __cpuid(query, static_cast<int>(0x80000000u));
    const unsigned int max_ext = static_cast<unsigned int>(query[0]);
    if (max_ext < 0x80000004u) {
        return "unknown";
    }
    char brand[49];
    for (unsigned int leaf = 0x80000002u; leaf <= 0x80000004u; ++leaf) {
        int regs[4] = {0, 0, 0, 0};
        __cpuid(regs, static_cast<int>(leaf));
        std::memcpy(brand + (leaf - 0x80000002u) * 16u, regs, 16);
    }
    brand[48] = '\0';
    return trim_spaces(brand);
}

struct core_info {
    uint32_t physical;
    uint32_t logical;
    bool ok;
};

core_info count_cores() {
    core_info info{0, 0, false};
    DWORD needed = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &needed);
    if (needed == 0) {
        return info;
    }
    std::vector<BYTE> buffer(needed);
    if (!GetLogicalProcessorInformationEx(
            RelationProcessorCore,
            reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(buffer.data()),
            &needed)) {
        return info;
    }
    DWORD offset = 0;
    while (offset < needed) {
        auto* entry = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(
            buffer.data() + offset);
        if (entry->Relationship == RelationProcessorCore) {
            info.physical += 1;
            uint64_t mask = 0;
            for (WORD group = 0; group < entry->Processor.GroupCount; ++group) {
                mask |= entry->Processor.GroupMask[group].Mask;
            }
            info.logical += static_cast<uint32_t>(std::bitset<64>(mask).count());
        }
        if (entry->Size == 0) {
            break;
        }
        offset += entry->Size;
    }
    info.ok = true;
    return info;
}

std::string os_version_string() {
    OSVERSIONINFOW vi{};
    vi.dwOSVersionInfoSize = sizeof(vi);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll == nullptr) {
        return "Windows (unknown)";
    }
    auto proc = GetProcAddress(ntdll, "RtlGetVersion");
    if (proc == nullptr) {
        return "Windows (unknown)";
    }
    auto rtl_get_version = reinterpret_cast<LONG(WINAPI*)(OSVERSIONINFOW*)>(
        reinterpret_cast<void*>(proc));
    if (rtl_get_version(&vi) != 0) {
        return "Windows (unknown)";
    }
    const std::string build = " (build " + std::to_string(vi.dwBuildNumber) + ")";
    if (vi.dwMajorVersion == 10) {
        return std::string(vi.dwBuildNumber >= 22000 ? "Windows 11" : "Windows 10") + build;
    }
    return "Windows " + std::to_string(vi.dwMajorVersion) + "." +
           std::to_string(vi.dwMinorVersion) + build;
}

std::string hostname_string() {
    char buffer[MAX_COMPUTERNAME_LENGTH + 1] = {};
    DWORD size = sizeof(buffer);
    if (GetComputerNameA(buffer, &size)) {
        return buffer;
    }
    return "unknown";
}

section probe_cpu_windows(bool& ok) {
    ok = false;
    section s{"cpu"};

    SYSTEM_INFO si{};
    GetSystemInfo(&si);

    MEMORYSTATUSEX mem{};
    mem.dwLength = sizeof(mem);
    if (!GlobalMemoryStatusEx(&mem)) {
        s.add_field("status", "failed");
        s.add_field("error", "GlobalMemoryStatusEx failed");
        return s;
    }
    const core_info cores = count_cores();

    s.add_field("status", "ok");
    s.add_field("os", os_version_string());
    s.add_field("hostname", hostname_string());
#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__)
    s.add_field("arch", "x86_64");
#elif defined(_M_ARM64) || defined(__aarch64__)
    s.add_field("arch", "arm64");
#else
    s.add_field("arch", "unknown");
#endif
    s.add_field("cpu_vendor", cpuid_vendor());
    s.add_field("cpu_brand", cpuid_brand());
    s.add_field("logical_processors", std::to_string(si.dwNumberOfProcessors));
    if (cores.ok) {
        s.add_field("physical_cores", std::to_string(cores.physical));
    }
    const unsigned int hw = std::thread::hardware_concurrency();
    s.add_field("hardware_concurrency", hw == 0 ? "unknown" : std::to_string(hw));
    s.add_field("total_physical_memory", fmt_gib(mem.ullTotalPhys));
    s.add_field("available_physical_memory", fmt_gib(mem.ullAvailPhys));
    s.add_field("total_virtual_memory", fmt_gib(mem.ullTotalPageFile));
    s.add_field("page_size", std::to_string(si.dwPageSize) + " bytes");
    s.add_field("allocation_granularity", std::to_string(si.dwAllocationGranularity) + " bytes");

    ok = true;
    return s;
}

#else

std::string read_proc_match(const char* path, const char* prefix) {
    std::ifstream in(path);
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind(prefix, 0) == 0) {
            return trim_spaces(line.substr(std::strlen(prefix)));
        }
    }
    return "";
}

section probe_cpu_linux(bool& ok) {
    ok = false;
    section s{"cpu"};

    utsname un{};
    if (uname(&un) == 0) {
        s.add_field("os", std::string(un.sysname) + " " + un.release);
        s.add_field("arch", un.machine);
    }
    const std::string pretty = read_proc_match("/etc/os-release", "PRETTY_NAME=");
    if (!pretty.empty()) {
        s.add_field("os_release", strip_quotes(pretty));
    }
    char host[256] = {};
    if (gethostname(host, sizeof(host)) == 0) {
        s.add_field("hostname", host);
    }
    const std::string model = read_proc_match("/proc/cpuinfo", "model name\t: ");
    s.add_field("cpu_brand", model.empty() ? "unknown" : model);
    long online = sysconf(_SC_NPROCESSORS_ONLN);
    s.add_field("logical_processors", online > 0 ? std::to_string(online) : "unknown");
    const long page_size = sysconf(_SC_PAGESIZE);
    const long phys_pages = sysconf(_SC_PHYS_PAGES);
    if (page_size > 0 && phys_pages > 0) {
        s.add_field("total_physical_memory", fmt_gib(static_cast<uint64_t>(page_size) *
                                                     static_cast<uint64_t>(phys_pages)));
    }
    const std::string mem_total = read_proc_match("/proc/meminfo", "MemTotal:");
    if (!mem_total.empty()) {
        s.add_field("meminfo_total", mem_total);
    }
    s.add_field("page_size", page_size > 0 ? std::to_string(page_size) + " bytes" : "unknown");

    s.add_field("status", "ok");
    ok = true;
    return s;
}

#endif

}

section probe_cpu(bool& ok) {
#ifdef _WIN32
    return probe_cpu_windows(ok);
#else
    return probe_cpu_linux(ok);
#endif
}

}
