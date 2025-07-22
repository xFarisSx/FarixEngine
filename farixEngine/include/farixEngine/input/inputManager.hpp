
#pragma once

#include "controller.hpp"
#include <SDL2/SDL.h>

namespace farixEngine {

class InputManager {
public:
  InputManager() = default;
  void pollEvents(bool &running, Controller *controller);
};
} // namespace farixEngine
