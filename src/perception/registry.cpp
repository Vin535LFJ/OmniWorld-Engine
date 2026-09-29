#include "perception/adapter.hpp"

namespace ow::detail {

std::map<std::string, std::function<std::unique_ptr<PerceptionAdapter>()>>&
perception_registry() {
  static std::map<std::string,
                  std::function<std::unique_ptr<PerceptionAdapter>()>> reg;
  return reg;
}

bool register_perception(
    const std::string& name,
    std::function<std::unique_ptr<PerceptionAdapter>()> factory) {
  perception_registry()[name] = std::move(factory);
  return true;
}

}  // namespace ow::detail

namespace ow {

std::vector<std::string> perception_names() {
  std::vector<std::string> names;
  for (const auto& [name, _] : detail::perception_registry()) {
    names.push_back(name);
  }
  return names;  // std::map iteration => sorted => deterministic
}

std::unique_ptr<PerceptionAdapter> create_perception(const std::string& name) {
  const auto& reg = detail::perception_registry();
  auto it = reg.find(name);
  if (it == reg.end()) return nullptr;
  return it->second();
}

std::vector<std::unique_ptr<PerceptionAdapter>> create_all_perception() {
  std::vector<std::unique_ptr<PerceptionAdapter>> out;
  for (const std::string& n : perception_names()) {
    out.push_back(create_perception(n));
  }
  return out;
}

}  // namespace ow
