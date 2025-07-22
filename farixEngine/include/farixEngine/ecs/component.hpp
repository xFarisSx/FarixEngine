#pragma once

#include <SDL2/SDL.h>

#include "farixEngine/thirdparty/nlohmann/json.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace farixEngine {
class World;
using json = nlohmann::json;
using Entity = uint32_t;

struct ComponentSerializer {
  std::function<json(World &, Entity)> to_json;
  std::function<void(World &, Entity, const json &)> from_json;
  std::function<bool(World &, Entity)> has;
};

class ComponentSerializerRegistry {
  std::unordered_map<std::string, ComponentSerializer> serializers;

public:
  void registerSerializer(const std::string &name,
                          const ComponentSerializer &serializer);
  bool hasSerializer(const std::string &name) const;
  const ComponentSerializer &getSerializer(const std::string &name) const;
  const std::unordered_map<std::string, ComponentSerializer> &getAll() const;
};

struct IComponentStorage {
  virtual ~IComponentStorage() = default;
  virtual void clear() = 0;
};

template <typename T> class ComponentStorage : public IComponentStorage {
public:
  std::unordered_map<Entity, T> components;

  void add(Entity entity, const T &component);
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
  ComponentSerializerRegistry serializerRegistry;

public:
  template <typename T>
  void registerComponent(
      const std::string &name, std::function<json(World &, Entity)> to_json,
      std::function<void(World &, Entity, const json &)> from_json);

  template <typename T> ComponentStorage<T> &getStorage();

  template <typename T> const ComponentStorage<T> &getStorage() const;

  ComponentSerializerRegistry &getSerializerRegistry();
  const ComponentSerializerRegistry &getSerializerRegistry() const;

  void clearStorages();
};
// MANAGER
template <typename T>
void ComponentManager::registerComponent(
    const std::string &name, std::function<json(World &, Entity)> to_json,
    std::function<void(World &, Entity, const json &)> from_json) {
  std::type_index type = std::type_index(typeid(T));
  storages[type] = std::make_shared<ComponentStorage<T>>();

  auto has_component = [this](World &world, Entity e) {
    return this->getStorage<T>().has(e);
  };

  ComponentSerializer c{to_json, from_json, has_component};
  serializerRegistry.registerSerializer(name, c);
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
void ComponentStorage<T>::add(Entity entity, const T &component) {
  components[entity] = component;
}

template <typename T> void ComponentStorage<T>::remove(Entity entity) {
  components.erase(entity);
}


} // namespace farixEngine

