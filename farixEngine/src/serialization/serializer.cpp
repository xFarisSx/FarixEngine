#include "farixEngine/serialization/serializer.hpp"
#include "farixEngine/core/engineRegistry.hpp"
#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/core/world.hpp"
#include "farixEngine/ecs/system.hpp"
#include "farixEngine/script/script.hpp"
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
    if (sysReg.exists(sys->name) && std::find(activeSystems.begin(), activeSystems.end(), sys->name) == activeSystems.end()) {
      activeSystems.push_back(sys->name);
    }
  }
  sceneJson["systems"] = activeSystems;

  auto &am = EngineServices::get().getAssetManager();

  sceneJson["assets"]["meshes"] = json::array();
  am.forEachAsset<Mesh>(
      [&](const std::string &uuid, std::shared_ptr<Mesh> mesh) {
        sceneJson["assets"]["meshes"].push_back(serializeAsset<Mesh>(mesh, am));
      });

  sceneJson["assets"]["textures"] = json::array();
  am.forEachAsset<Texture>([&](const std::string &uuid,
                               std::shared_ptr<Texture> tex) {
    sceneJson["assets"]["textures"].push_back(serializeAsset<Texture>(tex, am));
  });
  sceneJson["assets"]["materials"] = json::array();
  am.forEachAsset<Material>(
      [&](const std::string &uuid, std::shared_ptr<Material> mat) {
        sceneJson["assets"]["materials"].push_back(
            serializeAsset<Material>(mat, am));
      });

  sceneJson["assets"]["fonts"] = json::array();
  am.forEachAsset<Font>(
      [&](const std::string &uuid, std::shared_ptr<Font> font) {
        sceneJson["assets"]["fonts"].push_back(serializeAsset<Font>(font, am));
      });

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
  GameWorld &gworld = scene->gameWorld();
  auto &serializers = EngineServices::get()
                          .getEngineRegistry()
                          .getSerializerRegistry()
                          .getAll();

  world.clearStorages();

  for (const auto &entityJson : jsonData["entities"]) {
    Entity e = world.createEntity();
    gworld.registerExistingEntity(e);

    const auto &componentsJson = entityJson["components"];
    for (const auto &[componentName, componentData] : componentsJson.items()) {
      auto it = serializers.find(componentName);
      if (it != serializers.end()) {
        it->second.from_json(world, e, componentData);
      }
    }
  }

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

  auto &am = EngineServices::get().getAssetManager();

  if (jsonData.contains("assets") && jsonData["assets"].contains("meshes")) {
    for (auto &meshJson : jsonData["assets"]["meshes"]) {
      deserializeAsset<Mesh>(meshJson, am);
    }
  }

  if (jsonData.contains("assets") && jsonData["assets"].contains("textures")) {
    for (auto &texJson : jsonData["assets"]["textures"]) {
      deserializeAsset<Texture>(texJson, am);
    }
  }
  if (jsonData.contains("assets") && jsonData["assets"].contains("materials")) {
    for (auto &matJson : jsonData["assets"]["materials"]) {
      deserializeAsset<Material>(matJson, am);
    }
  }

  if (jsonData.contains("assets") && jsonData["assets"].contains("fonts")) {
    for (auto &fontJson : jsonData["assets"]["fonts"]) {
      deserializeAsset<Font>(fontJson, am);
    }
  }
}

void serializeEntityRecursive(World &world, Entity e, json &out,
                              std::vector<AssetID> &assetIds) {
  out["id"] = static_cast<uint32_t>(e);
  json componentsJson;

  for (const auto &[componentName, serializer] : EngineServices::get()
                                                     .getEngineRegistry()
                                                     .getSerializerRegistry()
                                                     .getAll()) {
    if (serializer.has(world, e)) {
      json serialized = serializer.to_json(world, e);

      componentsJson[componentName] = serialized;
      if (serialized.contains("uuid")) {
        // assetIds.push_back(serialized["uuid"].get<UUID>());
      } else if (serialized.contains("mesh")) {
        assetIds.push_back(serialized["mesh"].get<UUID>());
      } else if (serialized.contains("texture")) {
        assetIds.push_back(serialized["texture"].get<UUID>());
      } 
      else if (serialized.contains("material")) {
        assetIds.push_back(serialized["material"].get<UUID>());
      }
      else if (serialized.contains("font")) {
        assetIds.push_back(serialized["font"].get<UUID>());
      }
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
      serializeEntityRecursive(world, child, childJson, assetIds);
      out["children"].push_back(childJson);
    }
  }
}

void Serializer::savePrefab(GameObject &obj, const std::string &path) {
  World &world = *obj.getGameWorld()->getInternalWorld();
  Entity root = obj.getEntity();
  json rootJson;
  std::vector<AssetID> assetIds;
  auto &am = EngineServices::get().getAssetManager();
  serializeEntityRecursive(world, root, rootJson, assetIds);

  rootJson["assets"] = json::object();
  rootJson["assets"]["meshes"] = json::array();
  rootJson["assets"]["textures"] = json::array();
    rootJson["assets"]["materials"] = json::array();
  rootJson["assets"]["fonts"] = json::array();
  json &assetsJson = rootJson["assets"];
  json &meshesJson = assetsJson["meshes"];
  json &texturesJson = assetsJson["textures"];
    json &materialsJson = assetsJson["materials"];

  json &fontsJson = assetsJson["fonts"];
  for (AssetID uuid : assetIds) {

    auto asset = am.getRaw(uuid);

    if (auto mesh = std::dynamic_pointer_cast<Mesh>(asset)) {
      meshesJson.push_back(serializeAsset<Mesh>(mesh, am));
    } else if (auto tex = std::dynamic_pointer_cast<Texture>(asset)) {
      texturesJson.push_back(serializeAsset<Texture>(tex, am));
    } 
    else if (auto mat = std::dynamic_pointer_cast<Material>(asset)) {
      materialsJson.push_back(serializeAsset<Material>(mat, am));
    } 
    else if (auto font = std::dynamic_pointer_cast<Font>(asset)) {
      fontsJson.push_back(serializeAsset<Font>(font, am));
    }
  }

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

GameObject &Serializer::loadPrefab(GameWorld &gworld, const std::string &path) {
  std::ifstream in(path);
  json prefabJson;
  in >> prefabJson;
  World &world = *gworld.getInternalWorld();
  GameObject &root = gworld.registerExistingEntity(
      deserializeEntityRecursive(gworld, world, prefabJson));

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

  auto &am = EngineServices::get().getAssetManager();

  if (prefabJson.contains("assets") &&
      prefabJson["assets"].contains("meshes")) {
    for (auto &meshJson : prefabJson["assets"]["meshes"]) {
      deserializeAsset<Mesh>(meshJson, am);
    }
  }

  if (prefabJson.contains("assets") &&
      prefabJson["assets"].contains("textures")) {
    for (auto &texJson : prefabJson["assets"]["textures"]) {
      deserializeAsset<Texture>(texJson, am);
    }
  }
    if (prefabJson.contains("assets") &&
      prefabJson["assets"].contains("materials")) {
    for (auto &matJson : prefabJson["assets"]["materials"]) {
      deserializeAsset<Material>(matJson, am);
    }
  }

  if (prefabJson.contains("assets") && prefabJson["assets"].contains("fonts")) {
    for (auto &fontJson : prefabJson["assets"]["fonts"]) {
      deserializeAsset<Font>(fontJson, am);
    }
  }

  return root;
}

} // namespace farixEngine
