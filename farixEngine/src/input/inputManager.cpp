#include "farixEngine/input/inputManager.hpp"
#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/events/eventDispatcher.hpp"
#include "farixEngine/events/events.hpp"
#include "farixEngine/input/controller.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <iostream>
#include <optional>

namespace farixEngine {

static std::optional<Key> mapSDLScancodeToKey(SDL_Scancode sc) {
  switch (sc) {
  case SDL_SCANCODE_W:
    return Key::W;
  case SDL_SCANCODE_A:
    return Key::A;
  case SDL_SCANCODE_S:
    return Key::S;
  case SDL_SCANCODE_D:
    return Key::D;
  case SDL_SCANCODE_Q:
    return Key::Q;
  case SDL_SCANCODE_E:
    return Key::E;
  case SDL_SCANCODE_LEFT:
    return Key::Left;
  case SDL_SCANCODE_RIGHT:
    return Key::Right;
  case SDL_SCANCODE_UP:
    return Key::Up;
  case SDL_SCANCODE_DOWN:
    return Key::Down;
  case SDL_SCANCODE_SPACE:
    return Key::Space;
  case SDL_SCANCODE_LSHIFT:
  case SDL_SCANCODE_RSHIFT:
    return Key::Shift;
  case SDL_SCANCODE_ESCAPE:
    return Key::Escape;
  default:
    return std::nullopt;
  }
}

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

    else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
      SDL_Scancode sc = event.key.keysym.scancode;
      auto mappedKey = mapSDLScancodeToKey(sc);
      if (mappedKey) {
        controller->setKeyState(*mappedKey, event.type == SDL_KEYDOWN);

        auto &dispatcher = EngineServices::get().getEventDispatcher();

        if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
          KeyPressedEvent e(*mappedKey);
          dispatcher.emit(e);
        } else if (event.type == SDL_KEYUP) {
          KeyReleasedEvent e(*mappedKey);
          dispatcher.emit(e);
        }
      }
    }
  }

  // const Uint8 *keys = SDL_GetKeyboardState(nullptr);
  // controller->setKeyState(Key::W, keys[SDL_SCANCODE_W]);
  // controller->setKeyState(Key::A, keys[SDL_SCANCODE_A]);
  // controller->setKeyState(Key::S, keys[SDL_SCANCODE_S]);
  // controller->setKeyState(Key::D, keys[SDL_SCANCODE_D]);
  // controller->setKeyState(Key::Q, keys[SDL_SCANCODE_Q]);
  // controller->setKeyState(Key::E, keys[SDL_SCANCODE_E]);
  //
  // controller->setKeyState(Key::Left, keys[SDL_SCANCODE_LEFT]);
  // controller->setKeyState(Key::Right, keys[SDL_SCANCODE_RIGHT]);
  // controller->setKeyState(Key::Up, keys[SDL_SCANCODE_UP]);
  // controller->setKeyState(Key::Down, keys[SDL_SCANCODE_DOWN]);
  //
  // controller->setKeyState(Key::Space, keys[SDL_SCANCODE_SPACE]);
  // controller->setKeyState(Key::Shift, keys[SDL_SCANCODE_LSHIFT] ||
  //                                         keys[SDL_SCANCODE_RSHIFT]);
  // controller->setKeyState(Key::Escape, keys[SDL_SCANCODE_ESCAPE]);
}
} // namespace farixEngine
