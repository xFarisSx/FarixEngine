#pragma once

#include "farixEngine/script/scriptRegistry.hpp"
#include "farixEngine/ecs/componentSerializerRegistry.hpp"
#include "farixEngine/serialization/serializer.hpp"
#include <memory>
#include <unordered_map>

namespace farixEngine {

class EngineRegistry {
public:
    static EngineRegistry& get(); // Singleton accessor

    ScriptRegistry& getScriptRegistry();
    ComponentSerializerRegistry& getSerializerRegistry();

    void registerDefaults();

    void clear(); 

private:
    ScriptRegistry scriptRegistry;
    ComponentSerializerRegistry serializerRegistry;

    EngineRegistry() = default;
    EngineRegistry(const EngineRegistry&) = delete;
    EngineRegistry& operator=(const EngineRegistry&) = delete;
};

}

