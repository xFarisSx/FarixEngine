#pragma once
#include "farixEngine/API/gameObject.hpp"
#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/components/components.hpp"
#include "farixEngine/core/world.hpp"
#include "farixEngine/ecs/component.hpp"
#include "farixEngine/ecs/system.hpp"
#include "farixEngine/core/engineContext.hpp"
#include "farixEngine/input/controller.hpp"
#include "farixEngine/input/inputManager.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/renderer/renderer.hpp"
#include "farixEngine/scene/scene.hpp"
#include "farixEngine/scene/sceneManager.hpp"
#include "farixEngine/script/script.hpp"
#include "farixEngine/serialization/serializer.hpp"
#include "farixEngine/systems/systems.hpp"
#include "farixEngine/utils/uuid.hpp"

namespace farixEngine {
using Entity = uint32_t; 
class Engine {
public:
  Engine();
  ~Engine();

  void init(int width, int height, const char *title);
  void beginFrame(bool &running);
  void endFrame();
  void shutdown();

  Renderer *getRenderer() const { return renderer; }
  Controller *getController() const { return controller; }
  SceneManager *getSceneManager() const {return sceneManager;}
  InputManager *getInputManager() { return &inputManager; }
  EngineContext *getContext() const { return context; }

private:
  Renderer *renderer;
  Controller *controller = nullptr;
  SceneManager* sceneManager = nullptr;
  InputManager inputManager;
  EngineContext *context;
  bool _running = true;
};
} // namespace farixEngine
