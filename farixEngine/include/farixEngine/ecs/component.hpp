#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace farixEngine {
class World;
using Entity = uint32_t;

struct Component {
  virtual ~Component() = default;
  Component() = default;
};

struct IComponentStorage {
  virtual ~IComponentStorage() = default;
  virtual void clear() = 0;
  virtual void remove(Entity entity) = 0;
};

template <typename T> class ComponentStorage : public IComponentStorage {
public:
  std::unordered_map<Entity, T> components;

  T &add(Entity entity, const T &component);
  void remove(Entity entity);

  T &get(Entity entity);
  const T &get(Entity entity) const;

  bool has(Entity entity);
  bool has(Entity entity) const;

  void clear() { components.clear(); }
};
class ComponentManager {
private:
  std::unordered_map<std::type_index, std::shared_ptr<IComponentStorage>>
      storages;

public:
  template <typename T> void registerComponent();

  template <typename T> ComponentStorage<T> &getStorage();

  template <typename T> const ComponentStorage<T> &getStorage() const;
  template <typename T> bool hasStorage();

  std::unordered_map<std::type_index, std::shared_ptr<IComponentStorage>>
  getStorages();

  void clearStorages();
};
// MANAGER
template <typename T> void ComponentManager::registerComponent() {
  std::type_index type = std::type_index(typeid(T));
  storages[type] = std::make_shared<ComponentStorage<T>>();
}

template <typename T> ComponentStorage<T> &ComponentManager::getStorage() {
  std::type_index type = std::type_index(typeid(T));
  return *std::static_pointer_cast<ComponentStorage<T>>(storages[type]);
}

template <typename T>
const ComponentStorage<T> &ComponentManager::getStorage() const {
  std::type_index type = std::type_index(typeid(T));
  auto it = storages.find(type);
  if (it == storages.end()) {
    throw std::runtime_error("Component storage not found");
  }

  return *std::static_pointer_cast<ComponentStorage<T>>(it->second);
}

template <typename T> bool ComponentManager::hasStorage() {
  std::type_index type = std::type_index(typeid(T));
  auto it = storages.find(type);

  return !(it == storages.end());
}

// STORAGE
//

template <typename T> T &ComponentStorage<T>::get(Entity entity) {
  return components.at(entity);
}

template <typename T> const T &ComponentStorage<T>::get(Entity entity) const {
  return components.at(entity);
}

template <typename T> bool ComponentStorage<T>::has(Entity entity) {
  return components.find(entity) != components.end();
}

template <typename T> bool ComponentStorage<T>::has(Entity entity) const {
  return components.find(entity) != components.end();
}

template <typename T>
T &ComponentStorage<T>::add(Entity entity, const T &component) {
  components[entity] = component;
  return components[entity];
}

template <typename T> void ComponentStorage<T>::remove(Entity entity) {
  components.erase(entity);
}

} // namespace farixEngine
