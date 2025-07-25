#pragma once

#include "farixEngine/ecs/componentSerializerRegistry.hpp"
#include "farixEngine/ecs/systemRegistry.hpp"
#include "farixEngine/script/scriptRegistry.hpp"
#include "farixEngine/serialization/serializer.hpp"
#include <memory>
#include <unordered_map>

namespace farixEngine {

class EngineRegistry {
public:
  ScriptRegistry &getScriptRegistry();
  ComponentSerializerRegistry &getSerializerRegistry();
  SystemRegistry &getSystemRegistry();

  void registerDefaults();

  void clear();

private:
  ScriptRegistry scriptRegistry;
  ComponentSerializerRegistry serializerRegistry;
  SystemRegistry systemRegistry;
};

} // namespace farixEngine
