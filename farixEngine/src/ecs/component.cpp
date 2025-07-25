#include "farixEngine/ecs/component.hpp"

namespace farixEngine {


  std::unordered_map<std::type_index, std::shared_ptr<IComponentStorage>> ComponentManager::getStorages(){
  return storages;
}
void ComponentManager::clearStorages() {
  for (auto &[type, storagePtr] : storages) {
    storagePtr->clear();
  }
}
} // namespace farixEngine
