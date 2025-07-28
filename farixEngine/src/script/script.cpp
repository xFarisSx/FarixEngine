#include "farixEngine/script/script.hpp"
#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/core/world.hpp"
#include "farixEngine/scene/scene.hpp"

namespace farixEngine {

GameObject *Script::getGameObject() { return gameObject; }
GameWorld *Script::getGameWorld() { return &getScene()->gameWorld(); }
Scene *Script::getScene() const { return scene; }

} // namespace farixEngine
