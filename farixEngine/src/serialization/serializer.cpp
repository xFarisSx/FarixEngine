#include "farixEngine/serialization/serializer.hpp"
#include "farixEngine/core/engineRegistry.hpp"
#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/core/world.hpp"
#include "farixEngine/ecs/system.hpp"
#include "farixEngine/thirdparty/nlohmann/json.hpp"
#include <fstream>
#include <iostream>
using json = nlohmann::json;

namespace farixEngine {

void Serializer::saveScene(Scene *scene, const std::string &filepath) {
  if (!scene) {
    throw std::invalid_argument("Scene pointer is null.");
  }

  World &world = scene->world();
  json sceneJson;

  sceneJson["name"] = scene->name();

  sceneJson["activeCamera"] = world.getCamera();
  sceneJson["entities"] = json::array();

  for (Entity e : world.getEntities()) {
    json entityJson;
    entityJson["id"] = static_cast<uint32_t>(e);

    json componentsJson;
    for (const auto &[componentName, serializer] : EngineServices::get()
                                                       .getEngineRegistry()
                                                       .getSerializerRegistry()
                                                       .getAll()) {
      if (serializer.has(world, e)) {
        componentsJson[componentName] = serializer.to_json(world, e);
      }
    }

    entityJson["components"] = componentsJson;
    sceneJson["entities"].push_back(entityJson);
  }

  auto &sysReg = EngineServices::get().getEngineRegistry().getSystemRegistry();
  std::vector<std::string> activeSystems;
  for (auto &sys : scene->world().getSystems()) {
    if (sysReg.exists(sys->name)) {
      activeSystems.push_back(sys->name);
    }
  }
  sceneJson["systems"] = activeSystems;

  std::ofstream out(filepath);
  if (!out.is_open()) {
    throw std::runtime_error("Failed to open file for writing: " + filepath);
  }

  out << sceneJson.dump(2);
}
void Serializer::loadScene(Scene *scene, const std::string &filepath) {
  if (!scene) {
    throw std::invalid_argument("Scene pointer is null.");
  }

  std::ifstream in(filepath);
  if (!in.is_open()) {
    throw std::runtime_error("Failed to open scene file: " + filepath);
  }

  json jsonData;
  in >> jsonData;

  if (jsonData.contains("name")) {
    scene->setName(jsonData["name"].get<std::string>());
  }

  World &world = scene->world();
  auto &serializers = EngineServices::get()
                          .getEngineRegistry()
                          .getSerializerRegistry()
                          .getAll();

  world.clearStorages();

  for (const auto &entityJson : jsonData["entities"]) {
    Entity e = world.createEntity();

    const auto &componentsJson = entityJson["components"];
    for (const auto &[componentName, componentData] : componentsJson.items()) {
      auto it = serializers.find(componentName);
      if (it != serializers.end()) {
        it->second.from_json(world, e, componentData);
      }
    }
  }

  if (jsonData.contains("systems")) {
    const auto &systemList = jsonData["systems"];
    auto &registry =
        EngineServices::get().getEngineRegistry().getSystemRegistry();

    for (const auto &name : systemList) {
      if (!registry.exists(name)) {
        throw std::runtime_error("System '" + name.get<std::string>() +
                                 "' not registered.");
      }
      auto system = registry.create(name);
      scene->world().addSystem(system);
    }
  }

  if (jsonData.contains("activeCamera")) {
    world.setCameraEntity(jsonData["activeCamera"]);
  }
}

} // namespace farixEngine
