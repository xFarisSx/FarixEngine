#pragma once
#include "farixEngine/ecs/system.hpp"
#include "farixEngine/renderer/renderer.hpp"
#include "farixEngine/input/controller.hpp"
#include <memory>

namespace farixEngine {
class RenderSystem : public System {
public:
    RenderSystem(Renderer* renderer) : renderer(renderer) {}

    void start(World& world) override{};

    void update(World& world, float dt) override;

private:
    Renderer* renderer;
};

class ScriptSystem : public System {

    void start(World& world) override;

    void update(World& world,float  dt) override;

  

}; 

class HierarchySystem : public System {

    void start(World& world) override{};

    void update(World& world,float  dt) override;
    
    

};
 
class CameraControllerSystem : public System{
  public:
  CameraControllerSystem(Controller* ctrl) : controller(ctrl) {}

  void start(World& world) override{};

  void update(World& world, float dt) override;

  private:
    Controller* controller;
};
}   
