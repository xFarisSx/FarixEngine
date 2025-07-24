#pragma once
#include "farixEngine/core/engine.hpp"
#include "farixEngine/scene/sceneManager.hpp"
namespace farixEngine {

class Application {
public:
  virtual ~Application() = default;

  virtual void onUpdate(float dt) = 0;
  virtual void onStart() = 0;
  void run(int width, int height, const char *title);
  SceneManager &getSceneManager();

protected:
  void update(float dt);

protected:
  Engine engine;
  bool running = true;
};

} // namespace farixEngine
