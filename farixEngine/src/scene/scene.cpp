
#include "farixEngine/scene/scene.hpp"
#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/systems/systems.hpp"
#include <memory>
#include <string>

namespace farixEngine {
Scene::Scene(const std::string &name, const std::string &path)
    : _name(name), _path(path), _gameWorld(_world) {}

World &Scene::world() { return _world; }
GameWorld &Scene::gameWorld() { return _gameWorld; }
void Scene::onLoad() {
  _world.clearStorages();
  _world.clearSystems();
  _world.registerDefaults();
  _world.addSystem(std::make_shared<RenderSystem>());
  _world.addSystem(std::make_shared<ScriptSystem>());
  _world.addSystem(
      std::make_shared<CameraControllerSystem>());
  _world.addSystem(std::make_shared<HierarchySystem>());
  _world.addSystem(std::make_shared<PhysicsSystem>());
  _world.addSystem(std::make_shared<StateSystem>());
  _world.addSystem(std::make_shared<LifetimeSystem>());
  _world.addSystem(std::make_shared<TimerSystem>());
  _world.addSystem(std::make_shared<AudioSystem>());
  _world.addSystem(std::make_shared<CollisionSystem>());
}

const std::string &Scene::name() const { return _name; }
const std::string &Scene::path() const { return _path; }
void Scene::setName(const std::string &name) { _name = name; }
void Scene::setPath(const std::string &name) { _path = name; }

} // namespace farixEngine
