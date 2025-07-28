#include "farixEngine/utils/keyToString.hpp"

namespace farixEngine::utils {

const char *keyToString(Key key) {
  switch (key) {
  case Key::W:
    return "W";
  case Key::A:
    return "A";
  case Key::S:
    return "S";
  case Key::D:
    return "D";
  case Key::Q:
    return "Q";
  case Key::E:
    return "E";
  case Key::Left:
    return "Left";
  case Key::Right:
    return "Right";
  case Key::Up:
    return "Up";
  case Key::Down:
    return "Down";
  case Key::Space:
    return "Space";
  case Key::Shift:
    return "Shift";
  case Key::Escape:
    return "Escape";
  default:
    return "Unknown";
  }
}
} // namespace farixEngine::utils
