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
    EngineContext* getContext(); 
  void setContext(EngineContext* context); 

private:
    EngineRegistry engineRegistry;
      EngineContext* engineContext = nullptr;

    EngineServices() = default;
    EngineServices(const EngineServices&) = delete;
    EngineServices& operator=(const EngineServices&) = delete;
};

}

