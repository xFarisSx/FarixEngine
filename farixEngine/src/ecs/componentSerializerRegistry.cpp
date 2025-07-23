#include "farixEngine/ecs/componentSerializerRegistry.hpp"

namespace farixEngine {


bool ComponentSerializerRegistry::hasSerializer(const std::string &name) const {
  return serializers.find(name) != serializers.end();
}

const ComponentSerializer &
ComponentSerializerRegistry::getSerializer(const std::string &name) const {
  return serializers.at(name);
}

const std::unordered_map<std::string, ComponentSerializer> &
ComponentSerializerRegistry::getAll() const {
  return serializers;
}
} 
