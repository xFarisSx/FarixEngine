
#include "farixEngine/ecs/systemRegistry.hpp"

namespace farixEngine {

bool SystemRegistry::exists(const std::string &name) const {
  return factories.find(name) != factories.end();
}

std::shared_ptr<System> SystemRegistry::create(const std::string &name) const {
  auto it = factories.find(name);
  if (it == factories.end()) {
    return nullptr;
  }
  return it->second();
}

} // namespace farixEngine
