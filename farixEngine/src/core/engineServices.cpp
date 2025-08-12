#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/core/engineRegistry.hpp"
#include "farixEngine/scene/scene.hpp"
#include "farixEngine/scene/sceneManager.hpp"
 
namespace farixEngine {

EngineServices &EngineServices::get() {
  static EngineServices instance;
  return instance;
}
AssetManager &EngineServices::getAssetManager() { return assetManager; }
EngineRegistry &EngineServices::getEngineRegistry() { return engineRegistry; }
EventDispatcher &EngineServices::getEventDispatcher() {
  return getContext()->sceneManager->currentScene()->getEventDispatcher();
}
EngineContext *EngineServices::getContext() { return engineContext; }
void EngineServices::setContext(EngineContext *context) {
  engineContext = context;
}

} // namespace farixEngine  
