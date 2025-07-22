#include "farixEngine/application/application.hpp"
#include <chrono>

namespace farixEngine {

void Application::run(int width, int height, const char *title) {
  engine.init(width, height, title);
  sceneManager.setContext(engine.getContext());


  onStart();
  sceneManager.currentScene()->world().startSystems();
  auto last = std::chrono::high_resolution_clock::now();
  while (running) {
    auto now = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float>(now - last).count();
    last = now;

    update(dt);
  }

  engine.shutdown();
}

void Application::update(float dt) {

  engine.beginFrame(running);

  sceneManager.currentScene()->world().updateSystems(dt);
  onUpdate(dt);

  engine.endFrame();
}

SceneManager& Application::getSceneManager(){
  return sceneManager;
}
} // namespace farixEngine 
