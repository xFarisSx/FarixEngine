#include "farixEngine/scene/sceneManager.hpp"
#include "farixEngine/serialization/serializer.hpp"
#include <string>

namespace farixEngine {

Scene &SceneManager::createScene(const std::string &name, bool activate) {
  auto scene = std::make_unique<Scene>(name);
  scene->onLoad(ctx);
  Scene &ref = *scene;
  scenes[name] = std::move(scene);

  if (activate)
    activateScene(name);

  return ref;
}
Scene &SceneManager::loadSceneFromFile(const std::string &filePath,
                                       bool activate) {
  auto scene = std::make_unique<Scene>();
  scene->onLoad(ctx);

  Serializer::loadScene(scene.get(), filePath);


  std::cout << "Loaded scene name: '" << scene->name() << "'" << std::endl;
  const std::string name = scene->name();
  scenes[name] = std::move(scene);

  if (activate)
    activateScene(name);

  return *scenes[name];
}
void SceneManager::saveCurrentScene(const std::string &filePath) {
  Serializer::saveScene(activeScene, filePath);
}
void SceneManager::saveScene(const std::string &name,
                             const std::string &filePath) {
  Serializer::saveScene(&getScene(name), filePath);
}
Scene &SceneManager::getScene(const std::string &name) { return *scenes[name]; }

Scene &SceneManager::activateScene(const std::string &name) {
  activeScene = &getScene(name);
  return *activeScene;
}
void SceneManager::unloadScene(const std::string &name) { scenes.erase(name); }

Scene *SceneManager::currentScene() { return activeScene; }
GameWorld *SceneManager::currentGameWorld() {
  return &activeScene->gameWorld();
}
World *SceneManager::currentWorld() { return &activeScene->world(); }
void SceneManager::setContext(EngineContext *_ctx) { ctx = _ctx; }
} // namespace farixEngine
