
#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "farixEngine/thirdparty/nlohmann/json.hpp"

#include "farixEngine/components/components.hpp"
#include "farixEngine/core/engineContext.hpp"
#include "farixEngine/ecs/component.hpp"
#include "farixEngine/ecs/system.hpp"
#include "farixEngine/script/scriptRegistry.hpp"

namespace farixEngine {

class Script;
using ScriptPtr = std::shared_ptr<Script>;

class World {
public:
  World();
  World(const World &) = delete;
  World &operator=(const World &) = delete;

  using Entity = uint32_t;

  Entity createEntity();

  void setCameraEntity(Entity c);
  Entity getCamera() const;

  void updateSystems(float dt);
  void startSystems();
  void addSystem(std::shared_ptr<System> system);

  void setParent(Entity child, Entity parent);
  void removeParent(Entity child);
  void removeChild(Entity parent, Entity child);
  void removeAllChildren(Entity parent);
  std::vector<Entity> getChildren(Entity parent) const;

  void registerDefaults();
  void clearStorages();
  void clearSystems();

  template <typename T> void registerComponent();

  void loadScene(const std::string &filepath);
  void saveScene(const std::string &filepath);

  template <typename T> T &addComponent(Entity entity, const T &component);

  template <typename T> T &addComponent(Entity entity);

  template <typename T, typename... Args,
            typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  T &emplaceComponent(Entity entity, Args &&...args);

  template <typename... T> void addComponents(Entity entity);

  template <typename... T, typename... Components>
  void addComponents(Entity entity, Components &&...components);

  template <typename T> void removeComponent(Entity entity);

  template <typename T> bool hasComponent(Entity entity);
  template <typename T> const bool hasComponent(Entity entity) const;

  template <typename T> T &getComponent(Entity entity);
  template <typename T> const T &getComponent(Entity entity) const;
  template <typename... Components> std::vector<Entity> view() const;

  void addScript(uint32_t entity, ScriptPtr script);

  const std::vector<Entity> &getEntities() const;

  template <typename... Tags>
  std::vector<Entity> getEntitiesByTags(Tags... tags) const;

  std::vector<Entity> getEntitiesByName(const std::string &name) const;

  std::string getEntityName(Entity entity) const;
  void setName(Entity entity, const std::string &name);

  void addTag(Entity entity, const std::string &tag);
  void removeTag(Entity entity, const std::string &tag);
  bool hasTag(Entity entity, const std::string &tag) const;

  template <typename... Tags> void addTags(Entity entity, Tags &&...tags);
  template <typename... Tags> void removeTags(Entity entity, Tags &&...tags);
  template <typename... Tags> bool hasTags(Entity entity, Tags &&...tags) const;

  void destroyEntity(Entity e);

  ComponentManager &getComponentManager();
  std::vector<std::shared_ptr<System>> getSystems();

private:
  std::vector<Entity> entities;
  Entity _nextEntity = 1;
  Entity _cameraE = 0;
  ComponentManager componentManager;
  SystemManager systemManager;
  EngineContext *context = nullptr;
};

template <typename... Tags>
std::vector<World::Entity> World::getEntitiesByTags(Tags... tags) const {
  std::vector<Entity> result;
  for (Entity entity : entities) {
    if (hasTags(entity, std::forward<Tags>(tags)...))
      result.push_back(entity);
  }
  return result;
}

template <typename... Tags> void World::addTags(Entity entity, Tags &&...tags) {
  (addTag(entity, std::forward<Tags>(tags)), ...);
}

template <typename... Tags>
void World::removeTags(Entity entity, Tags &&...tags) {
  (removeTag(entity, std::forward<Tags>(tags)), ...);
}

template <typename... Tags>
bool World::hasTags(Entity entity, Tags &&...tags) const {
  return (hasTag(entity, std::forward<Tags>(tags)) && ...);
}

template <typename T> void World::registerComponent() {
  if (!componentManager.hasStorage<T>())
    componentManager.registerComponent<T>();
}

template <typename T>
T &World::addComponent(Entity entity, const T &component) {
  return componentManager.getStorage<T>().add(entity, component);
}

template <>
inline TransformComponent &
World::addComponent<TransformComponent>(Entity e,
                                        const TransformComponent &transform) {
  auto &t = componentManager.getStorage<TransformComponent>().add(e, transform);
  if (!hasComponent<GlobalTransform>(e)) {
    componentManager.getStorage<GlobalTransform>().add(
        e, GlobalTransform());
  }
  return t;
}

template <typename T> T &World::addComponent(Entity entity) {
  return componentManager.getStorage<T>().add(entity, T{});
}

template <>
inline TransformComponent &World::addComponent<TransformComponent>(Entity e) {
  auto &t = componentManager.getStorage<TransformComponent>().add(
      e, TransformComponent{});
  if (!hasComponent<GlobalTransform>(e)) {
    componentManager.getStorage<GlobalTransform>().add(
        e, GlobalTransform{Mat4::identity()});
  }
  return t;
}

template <typename T, typename... Args, typename>
T &World::emplaceComponent(Entity entity, Args &&...args) {
  T component(std::forward<Args>(args)...);
  return addComponent<T>(entity, component);
}

template <typename... T> void World::addComponents(Entity entity) {
  (addComponent<T>(entity), ...);
}

template <typename... T, typename... Components>
void World::addComponents(Entity entity, Components &&...components) {
  (addComponent<T>(entity, std::forward<Components>(components)), ...);
}

template <typename T> void World::removeComponent(Entity entity) {
  componentManager.getStorage<T>().remove(entity);
}

template <typename T> bool World::hasComponent(Entity entity) {
  return componentManager.getStorage<T>().has(entity);
}
template <typename T> const bool World::hasComponent(Entity entity) const {
  return componentManager.getStorage<T>().has(entity);
}
template <typename T> T &World::getComponent(Entity entity) {
  return componentManager.getStorage<T>().get(entity);
}

template <typename T> const T &World::getComponent(Entity entity) const {
  return componentManager.getStorage<T>().get(entity);
}

template <typename... Components>
std::vector<World::Entity> World::view() const {
  std::vector<Entity> result;
  for (Entity e : entities) {
    if ((hasComponent<Components>(e) && ...)) {
      result.push_back(e);
    }
  }
  return result;
}

} // namespace farixEngine
