#include "farixEngine/core/engineRegistry.hpp"
#include "farixEngine/core/engineServices.hpp"
namespace farixEngine {

EngineServices &EngineServices::get() {
  static EngineServices instance;
  return instance;
}

EngineRegistry &EngineServices::getEngineRegistry() { return engineRegistry; }

EngineContext* EngineServices::getContext(){return engineContext;}
void EngineServices::setContext(EngineContext* context){engineContext=context;}

} 
