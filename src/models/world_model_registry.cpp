#include "models/world_model.hpp"

namespace ow::detail {

std::map<std::string, std::function<std::unique_ptr<WorldModelAdapter>()>>&
world_model_registry() {
  static std::map<std::string,
                  std::function<std::unique_ptr<WorldModelAdapter>()>> reg;
  return reg;
}

bool register_world_model(
    const std::string& name,
    std::function<std::unique_ptr<WorldModelAdapter>()> factory) {
  world_model_registry()[name] = std::move(factory);
  return true;
}

}  // namespace ow::detail

namespace ow {

std::vector<std::string> world_model_names() {
  std::vector<std::string> names;
  for (const auto& [name, _] : detail::world_model_registry()) {
    names.push_back(name);
  }
  return names;  // std::map => sorted => deterministic
}

std::unique_ptr<WorldModelAdapter> create_world_model(const std::string& name) {
  const auto& reg = detail::world_model_registry();
  auto it = reg.find(name);
  if (it == reg.end()) return nullptr;
  return it->second();
}

std::vector<std::unique_ptr<WorldModelAdapter>> create_all_world_models() {
  std::vector<std::unique_ptr<WorldModelAdapter>> out;
  for (const std::string& n : world_model_names()) {
    out.push_back(create_world_model(n));
  }
  return out;
}

}  // namespace ow
