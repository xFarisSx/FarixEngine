#include "farixEngine/core/engine.hpp"
#include "farixEngine/core/engineContext.hpp"
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
  inputManager = InputManager();
  context->controller = controller;
  context->renderer = renderer;

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
