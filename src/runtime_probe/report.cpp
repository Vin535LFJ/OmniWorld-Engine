#include "report.hpp"

#include <chrono>
#include <cstdio>
#include <ctime>

namespace ow {

section& section::add_field(std::string key, std::string value) {
    fields.push_back(field{std::move(key), std::move(value)});
    return *this;
}

section& section::add_child(std::string child_name) {
    section child;
    child.name = std::move(child_name);
    children.push_back(std::move(child));
    return children.back();
}

std::string now_iso8601_utc() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    const std::time_t t = system_clock::to_time_t(now);
    const int ms = static_cast<int>(
        duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000);
    std::tm tm_value{};
#ifdef _WIN32
    gmtime_s(&tm_value, &t);
#else
    gmtime_r(&t, &tm_value);
#endif
    char buffer[40];
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
                  tm_value.tm_year + 1900, tm_value.tm_mon + 1, tm_value.tm_mday,
                  tm_value.tm_hour, tm_value.tm_min, tm_value.tm_sec, ms);
    return buffer;
}

std::string compiler_string() {
#if defined(_MSC_VER)
    return "MSVC " + std::to_string(_MSC_VER);
#elif defined(__clang__)
    return std::string("Clang ") + __clang_version__;
#elif defined(__GNUC__)
    return "GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) +
           "." + std::to_string(__GNUC_PATCHLEVEL__);
#else
    return "unknown";
#endif
}

std::string fmt_fixed(double value, int precision) {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%.*f", precision, value);
    return buffer;
}

std::string fmt_gib(uint64_t bytes) {
    return fmt_fixed(static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0), 2) + " GiB";
}

std::string fmt_mib(uint64_t bytes) {
    return fmt_fixed(static_cast<double>(bytes) / (1024.0 * 1024.0), 1) + " MiB";
}

namespace {

void print_section_text(const section& s, int depth) {
    const std::string indent(static_cast<size_t>(depth) * 2, ' ');
    std::printf("%s[%s]\n", indent.c_str(), s.name.c_str());
    for (const field& f : s.fields) {
        std::printf("%s  %s: %s\n", indent.c_str(), f.key.c_str(), f.value.c_str());
    }
    for (const section& c : s.children) {
        print_section_text(c, depth + 1);
    }
}

void write_json_string(const std::string& value) {
    std::fputc('"', stdout);
    for (const char c : value) {
        const unsigned char u = static_cast<unsigned char>(c);
        switch (u) {
            case '"': std::fputs("\\\"", stdout); break;
            case '\\': std::fputs("\\\\", stdout); break;
            case '\n': std::fputs("\\n", stdout); break;
            case '\r': std::fputs("\\r", stdout); break;
            case '\t': std::fputs("\\t", stdout); break;
            default:
                if (u < 0x20) {
                    std::printf("\\u%04x", u);
                } else {
                    std::fputc(u, stdout);
                }
        }
    }
    std::fputc('"', stdout);
}

void write_section_json(const section& s) {
    std::fputs("{\n  \"name\": ", stdout);
    write_json_string(s.name);
    std::fputs(",\n  \"fields\": {", stdout);
    for (size_t i = 0; i < s.fields.size(); ++i) {
        std::fputs(i == 0 ? "\n    " : ",\n    ", stdout);
        write_json_string(s.fields[i].key);
        std::fputs(": ", stdout);
        write_json_string(s.fields[i].value);
    }
    std::fputs(s.fields.empty() ? "" : "\n  ", stdout);
    std::fputs("},\n  \"children\": [", stdout);
    for (size_t i = 0; i < s.children.size(); ++i) {
        std::fputs(i == 0 ? "\n    " : ",\n    ", stdout);
        write_section_json(s.children[i]);
    }
    std::fputs(s.children.empty() ? "" : "\n  ", stdout);
    std::fputs("]\n}", stdout);
}

}

void print_text(const section& root) {
    print_section_text(root, 0);
}

void print_json(const section& root) {
    write_section_json(root);
    std::fputc('\n', stdout);
}

}
