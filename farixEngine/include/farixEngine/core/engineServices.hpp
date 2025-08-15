#pragma once

#include "farixEngine/core/engineRegistry.hpp"
#include "farixEngine/events/eventDispatcher.hpp"
#include <memory>
#include <unordered_map>

namespace farixEngine {

class EngineServices {
public:
    static EngineServices& get(); // Singleton accessor

    EngineRegistry& getEngineRegistry();
    EventDispatcher& getEventDispatcher();
    AssetManager& getAssetManager();
    EngineContext* getContext(); 
  void setContext(EngineContext* context); 

private:
    EngineRegistry engineRegistry;
    AssetManager assetManager;
      EngineContext* engineContext = nullptr;

    EngineServices() = default;
    EngineServices(const EngineServices&) = delete;
    EngineServices& operator=(const EngineServices&) = delete;
};

}
 
