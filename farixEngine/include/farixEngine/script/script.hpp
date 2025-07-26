#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "farixEngine/API/gameObject.hpp"

#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/core/world.hpp"

namespace farixEngine {
class Scene;
class Script {
  friend class ScriptSystem;

private:
  bool started = false;

protected:
  Scene *scene = nullptr;
  Entity e = 0;
  World *world = nullptr;
    GameObject* gameObject = nullptr;

public:
  std::string name = "Script";

  Script(const std::string &scriptName) : name(scriptName) {}

  virtual ~Script() = default;

  virtual void onStart() {}

  virtual void onUpdate(float dt) {}
  virtual void onCreate(GameObject* obj,Scene *scene) {
        this->gameObject = obj;
    this->scene = scene;
    std::cout << "hi from" << name << "\n";
  }

  void setContext(Entity entity, World *w) {
    world = w;
    e = entity;
  }

  template <typename T> T &getComponent();
  GameObject* getGameObject();
  GameWorld *getGameWorld();
  Scene *getScene() const;
};

template <typename T> T &Script::getComponent() {
  return getGameObject()->getComponent<T>();
}

} // namespace farixEngine
