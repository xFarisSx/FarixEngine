
#include "farixEngine/core/world.hpp"

#include "farixEngine/components/components.hpp"
#include "farixEngine/core/engineRegistry.hpp"
#include "farixEngine/ecs/system.hpp"
#include "farixEngine/script/script.hpp"
#include "farixEngine/serialization/serializer.hpp"
#include "farixEngine/thirdparty/nlohmann/json.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace farixEngine {
World::World() { registerDefaults(); }
World::Entity World::createEntity() {
  Entity id = _nextEntity++;
  entities.push_back(id);
  return id;
}
const std::vector<Entity> &World::getEntities() const { return entities; }
void World::clearStorages() {
  componentManager.clearStorages();
  entities.clear();
  _nextEntity = 1;
  _cameraE = 0;
}

void World::setCameraEntity(Entity c) { _cameraE = c; }

World::Entity World::getCamera() const { return _cameraE; }

std::vector<World::Entity>
World::getEntitiesByName(const std::string &name) const {
  std::vector<Entity> result;
  for (Entity e : entities) {
    if (getEntityName(e) == name) {
      result.push_back(e);
    }
  }
  return result;
}

std::string World::getEntityName(Entity entity) const {
  if (!hasComponent<Metadata>(entity))
    return "";
  return getComponent<Metadata>(entity).name;
}

void World::updateSystems(float dt) { systemManager.updateAll(*this, dt); }

void World::startSystems() { systemManager.startAll(*this); }

void World::addSystem(std::shared_ptr<System> system) {
  systemManager.addSystem(std::move(system));
}

void World::addScript(uint32_t entity, ScriptPtr script) {
  script->setContext(entity, this);

  if (hasComponent<ScriptComponent>(entity)) {
    ScriptComponent &sc = getComponent<ScriptComponent>(entity);
    sc.scripts.push_back(script);
  } else {
    ScriptComponent sc;
    sc.scripts.push_back(script);
    addComponent(entity, sc);
  }
}

std::vector<std::shared_ptr<System>> World::getSystems() {
  return systemManager.getAll();
}

void World::setName(Entity entity, const std::string &name) {
  if (!hasComponent<Metadata>(entity)) {
    addComponent(entity, Metadata{});
  }
  getComponent<Metadata>(entity).name = name;
}

void World::addTag(Entity entity, const std::string &tag) {
  if (!hasComponent<Metadata>(entity)) {
    addComponent(entity, Metadata{});
  }
  auto &tags = getComponent<Metadata>(entity).tags;
  if (std::find(tags.begin(), tags.end(), tag) == tags.end())
    tags.push_back(tag);
}

void World::removeTag(Entity entity, const std::string &tag) {
  if (!hasComponent<Metadata>(entity)) {
    addComponent(entity, Metadata{});
    return;
  }
  auto &tags = getComponent<Metadata>(entity).tags;
  auto it = std::find(tags.begin(), tags.end(), tag);
  if (it != tags.end())
    tags.erase(it);
}

bool World::hasTag(Entity entity, const std::string &tag) const {
  if (!hasComponent<Metadata>(entity))
    return false;
  const auto &tags = getComponent<Metadata>(entity).tags;
  return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

void World::setParent(Entity child, Entity parent) {
  if (!hasComponent<ChildrenComponent>(parent)) {
    addComponent(parent, ChildrenComponent{});
  }
  auto &children = getComponent<ChildrenComponent>(parent).children;
  if (std::find(children.begin(), children.end(), child) == children.end()) {
    children.push_back(child);
  }
  if (!hasComponent<ParentComponent>(child)) {
    addComponent(child, ParentComponent{parent});
  } else {
    getComponent<ParentComponent>(child).parent = parent;
  }
}

void World::removeParent(Entity child) {
  if (!hasComponent<ParentComponent>(child))
    return;
  Entity parent = getComponent<ParentComponent>(child).parent;
  if (hasComponent<ChildrenComponent>(parent)) {
    auto &children = getComponent<ChildrenComponent>(parent).children;
    children.erase(std::remove(children.begin(), children.end(), child),
                   children.end());
  }
  removeComponent<ParentComponent>(child);
}

void World::removeChild(Entity parent, Entity child) {
  if (!hasComponent<ChildrenComponent>(parent))
    return;
  auto &children = getComponent<ChildrenComponent>(parent).children;
  children.erase(std::remove(children.begin(), children.end(), child),
                 children.end());
  if (hasComponent<ParentComponent>(child) &&
      getComponent<ParentComponent>(child).parent == parent) {
    removeComponent<ParentComponent>(child);
  }
}

void World::removeAllChildren(Entity parent) {
  if (!hasComponent<ChildrenComponent>(parent))
    return;
  auto &children = getComponent<ChildrenComponent>(parent).children;
  for (Entity child : children) {
    if (hasComponent<ParentComponent>(child)) {
      removeComponent<ParentComponent>(child);
    }
  }
  children.clear();
}

std::vector<World::Entity> World::getChildren(Entity parent) const {
  if (hasComponent<ChildrenComponent>(parent)) {
    return getComponent<ChildrenComponent>(parent).children;
  }
  return {};
}
ComponentManager &World::getComponentManager() { return componentManager; }
void World::setContext(EngineContext *_context) { context = _context; }

EngineContext *World::getContext() const { return context; }

void World::registerDefaults() {
  registerComponent<Metadata>();
  registerComponent<TransformComponent>();
  registerComponent<GlobalTransform>();
  registerComponent<CameraComponent>();
  registerComponent<CameraControllerComponent>();
  registerComponent<MeshComponent>();
  registerComponent<MaterialComponent>();
  registerComponent<ScriptComponent>();
  registerComponent<ParentComponent>();
  registerComponent<ChildrenComponent>();
}

} // namespace farixEngine
