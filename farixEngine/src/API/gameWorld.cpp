

#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/core/engineContext.hpp"

namespace farixEngine {

GameWorld::GameWorld(World &world) : _world(world) {}

GameWorld::~GameWorld() {}

GameObject GameWorld::createGameObject() { return GameObject(_world); }

GameObject GameWorld::getGameObject(Entity entity) {
  return GameObject(_world, entity);
}

std::vector<GameObject> GameWorld::getAllGameObjects() {
  std::vector<GameObject> result;
  const auto &entities = _world.getEntities();
  result.reserve(entities.size());
  for (Entity e : entities) {
    result.emplace_back(_world, e);
  }
  return result;
}

std::vector<GameObject>
GameWorld::getGameObjectsByName(const std::string &name) {
  std::vector<GameObject> result;

  auto entities = _world.getEntitiesByName(const_cast<std::string &>(name));
  result.reserve(entities.size());
  for (Entity e : entities) {
    result.emplace_back(_world, e);
  }
  return result;
}

void GameWorld::setCamera(const GameObject &camera) {
  _world.setCameraEntity(camera.getEntity());
}

GameObject GameWorld::getCamera() {
  Entity camEntity = _world.getCamera();
  if (camEntity == 0) {
    return GameObject();
  }
  return GameObject(_world, camEntity);
}

void GameWorld::clear() { _world.clearStorages(); }

World &GameWorld::getInternalWorld() { return _world; }

const World &GameWorld::getInternalWorld() const { return _world; }

void GameWorld::addSystem(std::shared_ptr<System> sys) {
  _world.addSystem(sys);
}

} // namespace farixEngine
