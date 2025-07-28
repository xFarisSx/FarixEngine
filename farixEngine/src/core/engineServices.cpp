#include "farixEngine/core/engineRegistry.hpp"
#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/scene/sceneManager.hpp"
#include "farixEngine/scene/scene.hpp"
namespace farixEngine {

EngineServices &EngineServices::get() {
  static EngineServices instance;
  return instance;
}

EngineRegistry &EngineServices::getEngineRegistry() { return engineRegistry; }
EventDispatcher& EngineServices::getEventDispatcher(){
  return getContext()->sceneManager->currentScene()->getEventDispatcher();
} 
EngineContext* EngineServices::getContext(){return engineContext;}
void EngineServices::setContext(EngineContext* context){engineContext=context;}

} 
