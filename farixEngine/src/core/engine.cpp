#include "farixEngine/core/engine.hpp"
#include "farixEngine/core/engineContext.hpp"
#include "farixEngine/core/engineServices.hpp"
#include <farixEngine/scene/sceneManager.hpp>
#include <iostream>
namespace farixEngine {

Engine::Engine() {}

Engine::~Engine() { shutdown(); }

void Engine::init(int width, int height, const char *title) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
    exit(1);
  }

  context = new EngineContext();

  renderer = new Renderer(width, height, title);
  controller = new Controller();
  sceneManager = new SceneManager();
  inputManager = InputManager();
  context->controller = controller;
  context->renderer = renderer;
  context->sceneManager=sceneManager;
  EngineServices::get().setContext(context);
  std::cout << "Engine initialized\n";
}
 
void Engine::beginFrame(bool& running){
  inputManager.pollEvents(running, controller);

  renderer->clear();

} 
 
void Engine::endFrame(){
  renderer->present();

}
 
void Engine::shutdown() {
  _running = false;
  delete controller;
  controller = nullptr;
  delete renderer;
  renderer = nullptr;
  delete context;
  context = nullptr;

  SDL_Quit();
}
} // namespace farixEngine
