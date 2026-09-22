#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ow {

struct field {
    std::string key;
    std::string value;
};

struct section {
    std::string name;
    std::vector<field> fields;
    std::vector<section> children;

    section& add_field(std::string key, std::string value);
    section& add_child(std::string child_name);
};

std::string now_iso8601_utc();
std::string compiler_string();
std::string fmt_fixed(double value, int precision);
std::string fmt_gib(uint64_t bytes);
std::string fmt_mib(uint64_t bytes);

void print_text(const section& root);
void print_json(const section& root);

}
