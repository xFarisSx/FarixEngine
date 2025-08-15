#pragma once
#include "farixEngine/renderer/renderer.hpp"
namespace farixEngine {
struct Controller;
class SceneManager;

struct EngineContext {
  Controller *controller = nullptr;
  renderer::IRenderer *renderer = nullptr;
  SceneManager *sceneManager = nullptr;
};
} // namespace farixEngine
