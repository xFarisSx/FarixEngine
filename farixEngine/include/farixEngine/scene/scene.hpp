#pragma once
#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/core/world.hpp"
#include "farixEngine/events/eventDispatcher.hpp"
#include <string>

namespace farixEngine {

class Scene {
public:
  Scene(const std::string &name = "", const std::string &path = "");

  World &world();
  GameWorld &gameWorld();
  void onLoad();

  const std::string &name() const;
  const std::string &path() const;
  void setName(const std::string &name);
  void setPath(const std::string &name);

  EventDispatcher& getEventDispatcher();

private:
  std::string _name;
  std::string _path;
  World _world;
  GameWorld _gameWorld;
  EventDispatcher eventDispatcher;
  
};
} // namespace farixEngine
