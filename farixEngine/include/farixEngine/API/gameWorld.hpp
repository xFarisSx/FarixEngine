
#pragma once

#include "farixEngine/API/gameObject.hpp"
#include "farixEngine/core/engineContext.hpp"
#include "farixEngine/core/world.hpp"
#include <memory>
#include <string>
#include <vector>

namespace farixEngine {

class GameWorld {
  World &_world;

public:
  GameWorld(World &world);

  ~GameWorld();

  GameObject createGameObject();
  GameObject getGameObject(Entity entity);

  std::vector<GameObject> getAllGameObjects();
  std::vector<GameObject> getGameObjectsByName(const std::string &name);

  template <typename... Tags>
  std::vector<GameObject> getGameObjectsByTags(Tags &&...tags);

  void setCamera(const GameObject &camera);
  GameObject getCamera();

  void clear();

  World &getInternalWorld();
  const World &getInternalWorld() const;

  EngineContext* getContext();
  void addSystem(std::shared_ptr<System> sys);
  template<typename T>
  void registerComponent();

private:
  GameWorld(const GameWorld &) = delete;
  GameWorld &operator=(const GameWorld &) = delete;
};

template <typename... Tags>
std::vector<GameObject> GameWorld::getGameObjectsByTags(Tags &&...tags) {
  std::vector<GameObject> result;
  auto entities = _world.getEntitiesByTags(std::forward<Tags>(tags)...);
  result.reserve(entities.size());
  for (Entity e : entities) {
    result.emplace_back(_world, e);
  }
  return result;
}

template<typename T>
void GameWorld::registerComponent(){
  _world.registerComponent<T>();
}



} // namespace farixEngine
