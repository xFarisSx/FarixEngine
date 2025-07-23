#include "farixEngine/ecs/component.hpp"

namespace farixEngine {



void ComponentManager::clearStorages() {
  for (auto &[type, storagePtr] : storages) {
    storagePtr->clear();
  }
}
} // namespace farixEngine
