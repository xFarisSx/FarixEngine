#include "farixEngine/input/inputManager.hpp"
#include "farixEngine/input/controller.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <iostream>

namespace farixEngine {
void InputManager::pollEvents(bool &running, Controller *controller) {
  controller->resetMotion();

  SDL_Event event;
  while (SDL_PollEvent(&event)) {

    if (event.type == SDL_QUIT) {
      std::cout << "Received SDL_QUIT event, stopping engine.\n";
      running = false;
    }

    else if (event.type == SDL_MOUSEBUTTONDOWN &&
             event.button.button == SDL_BUTTON_RIGHT)
      controller->rightClick = true;

    else if (event.type == SDL_MOUSEBUTTONUP &&
             event.button.button == SDL_BUTTON_RIGHT)
      controller->rightClick = false;

    else if (event.type == SDL_MOUSEMOTION) {
      controller->dx = event.motion.xrel;
      controller->dy = event.motion.yrel;
      controller->inMotion = (controller->dx != 0 || controller->dy != 0);
    }
  }
 
  const Uint8 *keys = SDL_GetKeyboardState(nullptr);
  controller->setKeyState(Key::W, keys[SDL_SCANCODE_W]);
  controller->setKeyState(Key::A, keys[SDL_SCANCODE_A]);
  controller->setKeyState(Key::S, keys[SDL_SCANCODE_S]);
  controller->setKeyState(Key::D, keys[SDL_SCANCODE_D]);
  controller->setKeyState(Key::Q, keys[SDL_SCANCODE_Q]);
  controller->setKeyState(Key::E, keys[SDL_SCANCODE_E]);

  controller->setKeyState(Key::Left, keys[SDL_SCANCODE_LEFT]);
  controller->setKeyState(Key::Right, keys[SDL_SCANCODE_RIGHT]);
  controller->setKeyState(Key::Up, keys[SDL_SCANCODE_UP]);
  controller->setKeyState(Key::Down, keys[SDL_SCANCODE_DOWN]);

  controller->setKeyState(Key::Space, keys[SDL_SCANCODE_SPACE]);
  controller->setKeyState(Key::Shift, keys[SDL_SCANCODE_LSHIFT] ||
                                          keys[SDL_SCANCODE_RSHIFT]);
  controller->setKeyState(Key::Escape, keys[SDL_SCANCODE_ESCAPE]);
}
} // namespace farixEngine
