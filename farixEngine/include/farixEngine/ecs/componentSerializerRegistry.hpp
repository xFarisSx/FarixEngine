#pragma once

#include "farixEngine/thirdparty/nlohmann/json.hpp"
#include "farixEngine/core/world.hpp"
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace farixEngine {
using json = nlohmann::json;
using Entity = uint32_t;

struct ComponentSerializer {
  std::function<json(World &, Entity)> to_json;
  std::function<void(World &, Entity, const json &)> from_json;
  std::function<bool(World &, Entity)> has;
};

class ComponentSerializerRegistry {
  std::unordered_map<std::string, ComponentSerializer> serializers;

public:
  template<typename T>
  void registerSerializer(
      const std::string &name, std::function<json(World &, Entity)> to_json,
      std::function<void(World &, Entity, const json &)> from_json);

  bool hasSerializer(const std::string &name) const;
  const ComponentSerializer &getSerializer(const std::string &name) const;
  const std::unordered_map<std::string, ComponentSerializer> &getAll() const;
};

template<typename T>
void ComponentSerializerRegistry::registerSerializer(
    const std::string &name, std::function<json(World &, Entity)> to_json,
    std::function<void(World &, Entity, const json &)> from_json) {

  auto has_component = [this](World &world, Entity e) {
    return world.getComponentManager().getStorage<T>().has(e);
  };

  ComponentSerializer serializer{to_json, from_json, has_component};

  serializers[name] = serializer;
}
 

} // namespace farixEngine
