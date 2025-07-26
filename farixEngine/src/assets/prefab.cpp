#include "farixEngine/assets/prefab.hpp"
#include "farixEngine/API/gameObject.hpp"
#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/core/engineRegistry.hpp"
#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/script/script.hpp"
#include "farixEngine/thirdparty/nlohmann/json.hpp"
#include <fstream>
#include <iostream>
using json = nlohmann::json;

namespace farixEngine {

void serializeEntityRecursive(World &world, Entity e, json &out) {
  out["id"] = static_cast<uint32_t>(e);
  json componentsJson;

  for (const auto &[componentName, serializer] : EngineServices::get()
                                                     .getEngineRegistry()
                                                     .getSerializerRegistry()
                                                     .getAll()) {
    if (serializer.has(world, e)) {
      componentsJson[componentName] = serializer.to_json(world, e);
    }
  }

  out["components"] = componentsJson;

  if (world.hasComponent<Metadata>(e)) {
    const auto &meta = world.getComponent<Metadata>(e);
    if (!meta.prefab.empty()) {
      out["prefab"] = meta.prefab;
    }
  }

  if (world.hasComponent<ChildrenComponent>(e)) {
    const auto &children = world.getComponent<ChildrenComponent>(e).children;
    out["children"] = json::array();
    for (Entity child : children) {
      json childJson;
      serializeEntityRecursive(world, child, childJson);
      out["children"].push_back(childJson);
    }
  }
}

void Prefab::save(GameObject &obj, const std::string &path) {
  World &world = *obj.getGameWorld()->getInternalWorld();
  Entity root = obj.getEntity();
  json rootJson;
  serializeEntityRecursive(world, root, rootJson);

  std::ofstream out(path);
  if (!out.is_open()) {
    std::cerr << "Failed to save prefab to " << path << "\n";
    return;
  }

  out << rootJson.dump(2);
  out.close();
}

Entity deserializeEntityRecursive(GameWorld &gworld, World &world,
                                  const json &entityJson) {
  Entity e = world.createEntity();
  gworld.registerExistingEntity(e);
  if (entityJson.contains("components")) {
    const auto &componentsJson = entityJson["components"];
    for (const auto &[componentName, componentData] : componentsJson.items()) {
      const auto &deserializer = EngineServices::get()
                                     .getEngineRegistry()
                                     .getSerializerRegistry()
                                     .getSerializer(componentName);
      deserializer.from_json(world, e, componentData);
    }
  }

  if (entityJson.contains("components") &&
      entityJson["components"].contains("Metadata")) {
    auto &meta = world.addComponent<Metadata>(e);
    const auto &metaJson = entityJson["components"]["Metadata"];

    meta.name = metaJson.value("name", "Entity");

    if (metaJson.contains("tags") && metaJson["tags"].is_array()) {
      meta.tags.clear();
      for (const auto &tag : metaJson["tags"]) {
        if (tag.is_string()) {
          meta.tags.push_back(tag.get<std::string>());
        }
      }
    } else {
    }

    if (entityJson.contains("prefab") && entityJson["prefab"].is_string()) {
      meta.prefab = entityJson["prefab"].get<std::string>();
    } else if (metaJson.contains("prefab") && metaJson["prefab"].is_string()) {
      meta.prefab = metaJson["prefab"].get<std::string>();
    }
  }

  if (entityJson.contains("children")) {
    auto &children = world.addComponent<ChildrenComponent>(e);
    for (const auto &childJson : entityJson["children"]) {
      Entity child = deserializeEntityRecursive(gworld, world, childJson);
      children.children.push_back(child);

      if (!world.hasComponent<ParentComponent>(child)) {
        auto &parent = world.addComponent<ParentComponent>(child);
        parent.parent = e;
      }
    }
  }
 
  return e;
}

GameObject& Prefab::instantiate(GameWorld &gworld, const std::string &path) {
  std::ifstream in(path);
  json prefabJson;
  in >> prefabJson;
  World &world = *gworld.getInternalWorld();
  GameObject& root = gworld.registerExistingEntity(   deserializeEntityRecursive(gworld, world, prefabJson));

  for (auto &obj : gworld.getAllGameObjects()) {
    if (!obj->hasComponent<ScriptComponent>())
      continue;
    auto &scriptComp = obj->getComponent<ScriptComponent>();

    for (auto &script : scriptComp.scripts) {
      if (script) {
        script->onCreate(obj, gworld.getOwningScene());
      }
    }
  }
  return root;
}
} // namespace farixEngine
