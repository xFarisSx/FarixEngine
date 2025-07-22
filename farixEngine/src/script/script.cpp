#include "farixEngine/script/script.hpp"
#include "farixEngine/core/world.hpp"

namespace farixEngine {
void Script::setContext(uint32_t id, World *w) {
  entityId = id;
  world = w;
}

} // namespace farixEngine  
