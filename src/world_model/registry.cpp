#include "world_model/world_model.hpp"

#include <map>

namespace ow::world_model {
namespace {

std::map<std::string, Factory>& table() {
  static std::map<std::string, Factory> t;  // sorted by name = deterministic
  return t;
}

}  // namespace

bool reg(const char* name, Factory f) {
  table().emplace(std::string(name), f);
  return true;
}

std::unique_ptr<WorldModelAdapter> make(const std::string& name) {
  auto it = table().find(name);
  return it == table().end() ? nullptr : it->second();
}

std::vector<std::string> names() {
  std::vector<std::string> out;
  out.reserve(table().size());
  for (const auto& [k, _] : table()) out.push_back(k);
  return out;
}

}  // namespace ow::world_model
