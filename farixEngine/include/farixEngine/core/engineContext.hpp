#pragma once

namespace farixEngine{
struct Controller;
class Renderer;
class SceneManager;

struct EngineContext {
  Controller *controller = nullptr;
  Renderer *renderer = nullptr;
  SceneManager *sceneManager = nullptr;
};
}
