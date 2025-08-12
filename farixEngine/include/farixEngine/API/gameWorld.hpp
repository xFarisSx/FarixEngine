
#pragma once

#include "farixEngine/API/gameObject.hpp"
#include "farixEngine/core/engineContext.hpp"
#include "farixEngine/core/world.hpp"
#include <memory>
#include <string>
#include <vector>

namespace farixEngine {
class Scene;
class GameWorld {
  friend class Scene;

  World *_world = nullptr;
  Scene *owningScene = nullptr;
  std::unordered_map<Entity, GameObject> gameObjects;
  void setOwningScene(Scene *scene) { owningScene = scene; }

public: 
  Scene *getOwningScene() const { return owningScene; }
  GameWorld(World *world);

  ~GameWorld();

  GameObject &createGameObject();
  GameObject &createSprite3D(AssetID texture,
                             Vec3 size = {1, 1, 0});
  GameObject &createSprite2D(AssetID texture,
                             Vec3 size = {1, 1, 0});
  GameObject &registerExistingEntity(Entity e);
  GameObject &getGameObject(Entity entity);

  std::vector<GameObject *> getAllGameObjects();
  std::vector<GameObject *> getGameObjectsByName(const std::string &name);

  template <typename... Tags>
  std::vector<GameObject *> getGameObjectsByTags(Tags &&...tags);

  void setCamera(const GameObject &camera);
  GameObject &getCamera();

  void clear();
  void destroyObject(GameObject &go);

  World *getInternalWorld();
  const World *getInternalWorld() const;

  void addSystem(std::shared_ptr<System> sys);
  template <typename T> void registerComponent();

private:
  GameWorld(const GameWorld &) = delete;
  GameWorld &operator=(const GameWorld &) = delete;
};

template <typename... Tags>
std::vector<GameObject *> GameWorld::getGameObjectsByTags(Tags &&...tags) {
  std::vector<GameObject *> result;
  auto entities = _world->getEntitiesByTags(std::forward<Tags>(tags)...);
  result.reserve(entities.size());
  for (Entity e : entities) {
    result.push_back(&getGameObject(e));
  }
  return result;
}
template <typename T> void GameWorld::registerComponent() {
  _world->registerComponent<T>();
}

} // namespace farixEngine
