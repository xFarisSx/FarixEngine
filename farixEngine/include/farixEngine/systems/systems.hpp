#pragma once
#include "farixEngine/ecs/system.hpp"
#include "farixEngine/input/controller.hpp"
#include "farixEngine/renderer/renderer.hpp"
#include <memory>

namespace farixEngine {
class RenderSystem : public System {
public:
  RenderSystem(Renderer *renderer)
      : System("RenderSystem"), renderer(renderer) {}

  void start(World &world) override {};

  void update(World &world, float dt) override;

private:
  Renderer *renderer;
};

class ScriptSystem : public System {
public:
  ScriptSystem() : System("ScriptSystem") {}

  void start(World &world) override;

  void update(World &world, float dt) override;
};

class HierarchySystem : public System {
public:
  HierarchySystem() : System("HierarchySystem") {}

  void start(World &world) override {};

  void update(World &world, float dt) override;
};

class CameraControllerSystem : public System {
public:
  CameraControllerSystem(Controller *ctrl)
      : System("CameraControlSystem"), controller(ctrl) {}

  void start(World &world) override {};

  void update(World &world, float dt) override;

private:
  Controller *controller;
};

class PhysicsSystem : public System {
public:
  PhysicsSystem() : System("PhysicsSystem") {}

  void start(World &world) override {}

  void update(World &world, float dt) override;
};

class CollisionSystem : public System {
public:
  CollisionSystem() : System("CollisionSystem") {}

  void start(World &world) override {}

  void update(World &world, float dt) override;
};

class StateSystem : public System {
public:
  StateSystem() : System("StateSystem") {}

  void start(World &world) override {}

  void update(World &world, float dt) override;
};

class LifetimeSystem : public System {
public:
  LifetimeSystem() : System("LifetimeSystem") {}

  void start(World &world) override {}

  void update(World &world, float dt) override;
};

class AudioSystem : public System {
public:
  AudioSystem() : System("AudioSystem") {}

  void start(World &world) override {}
  void update(World &world, float dt) override;
};

class TimerSystem : public System {
public:
  TimerSystem() : System("TimerSystem") {}

  void start(World &world) override {}

  void update(World &world, float dt) override;

  void addTimer(World &world, Entity e, const std::string &name, float max,
                bool repeat);
  void removeTimer(World &world, Entity e, const std::string &name);
};

} // namespace farixEngine
