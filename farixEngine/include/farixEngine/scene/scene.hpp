#pragma once
#include <string>
#include "farixEngine/core/world.hpp"
#include "farixEngine/API/gameWorld.hpp"

namespace farixEngine {


  class Scene {
  public:
    Scene(const std::string& name="", const std::string& path = "");

    World& world();          
    GameWorld& gameWorld();   
    void onLoad(EngineContext*context);

    const std::string& name() const;
    const std::string& path() const;
    void setName(const std::string& name) ;

  private:
    std::string _name;
    std::string _path;
    World _world;
    GameWorld _gameWorld;
  };
}
