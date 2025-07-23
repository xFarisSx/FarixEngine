#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "farixEngine/API/gameObject.hpp"

#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/core/world.hpp"

namespace farixEngine {

class Script {
private:
  uint32_t entityId = 0;
  World *world = nullptr;


public:
  std::string name = "Script";

  Script(const std::string &scriptName) : name(scriptName) {}

  virtual ~Script() = default;

  virtual void start() {}

  virtual void update(float dt) {}

  void setContext(uint32_t id, World *w);

  template <typename T> T &getComponent();
  GameObject getGameObject();
  GameWorld getGameWorld();
};

template <typename T> T &Script::getComponent() {
  return world->template getComponent<T>(entityId);
}

} // namespace farixEngine
