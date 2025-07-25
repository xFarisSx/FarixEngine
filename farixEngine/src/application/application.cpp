#include "farixEngine/application/application.hpp"
#include "farixEngine/core/engineRegistry.hpp"
#include "farixEngine/core/engineServices.hpp"
#include <chrono>

namespace farixEngine {

void Application::run(int width, int height, const char *title) {
  engine.init(width, height, title);
  EngineServices::get().getEngineRegistry().registerDefaults();

  onStart();
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

  onUpdate(dt);
  getSceneManager().currentScene()->world().updateSystems(dt);

  engine.endFrame();
}

SceneManager &Application::getSceneManager() {
  return *engine.getSceneManager();
}
} // namespace farixEngine
