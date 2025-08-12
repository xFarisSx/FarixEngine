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
    if (sysReg.exists(sys->name)) {
      activeSystems.push_back(sys->name);
    }
  }
  sceneJson["systems"] = activeSystems;

  auto &am = EngineServices::get().getAssetManager();

  sceneJson["assets"]["meshes"] = json::array();
  am.forEachAsset<Mesh>(
      [&](const std::string &uuid, std::shared_ptr<Mesh> mesh) {
        json meshJson;
        meshJson["uuid"] = uuid;
        meshJson["name"] = am.findNameById(uuid).value_or("");
        meshJson["path"] = mesh->path;
        meshJson["type"] = mesh->type;
        meshJson["size"] = mesh->size;
        meshJson["sphereData"] = mesh->sphereData;

        sceneJson["assets"]["meshes"].push_back(meshJson);
      });

  sceneJson["assets"]["textures"] = json::array();
  am.forEachAsset<Texture>(
      [&](const std::string &uuid, std::shared_ptr<Texture> tex) {
        json texJson;
        texJson["uuid"] = uuid;
        texJson["name"] = am.findNameById(uuid).value_or("");
        texJson["path"] = tex->path;
        sceneJson["assets"]["textures"].push_back(texJson);
      });

  sceneJson["assets"]["fonts"] = json::array();
  am.forEachAsset<Font>(
      [&](const std::string &uuid, std::shared_ptr<Font> font) {
        json fontJson;
        fontJson["uuid"] = uuid;
        fontJson["name"] = am.findNameById(uuid).value_or("");
        fontJson["path"] = font->path;
        fontJson["ptsize"] = font->ptsize;
        sceneJson["assets"]["fonts"].push_back(fontJson);
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
      std::string uuid = meshJson["uuid"];
      std::string name = meshJson.value("name", "");
      std::string path = meshJson.value("path", "");
      std::string type = meshJson.value("type", "");
      Vec3 size = meshJson.value("size", Vec3(1.f));
      Vec3 sphereData = meshJson.value("sphereData", Vec3{1.0, 16.0, 32.0});

      std::shared_ptr<Mesh> mesh;
      if (type == "Obj")
        mesh = Mesh::loadFromObj(path, uuid);
      else if (type == "Box")
        mesh = Mesh::createBox(size[0], size[1], size[2], uuid);
      else if (type == "Sphere")
        mesh = Mesh::createSphere(sphereData[0], sphereData[1], sphereData[2],
                                  uuid);
      else if (type == "Sprite")
        mesh = Mesh::createQuad(size[0], uuid);

      am.add(mesh, name);
    }
  }

  if (jsonData.contains("assets") && jsonData["assets"].contains("textures")) {
    for (auto &texJson : jsonData["assets"]["textures"]) {
      std::string uuid = texJson["uuid"];
      std::string name = texJson.value("name", "");
      std::string path = texJson.value("path", "");

      auto tex = Texture::loadFromBmp(path, uuid);
      am.add(tex, name);
    }
  }

  if (jsonData.contains("assets") && jsonData["assets"].contains("fonts")) {
    for (auto &fontJson : jsonData["assets"]["fonts"]) {
      std::string uuid = fontJson["uuid"];
      std::string name = fontJson.value("name", "");
      std::string path = fontJson.value("path", "");
      int ptsize = fontJson.value("ptsize", 24);

      auto font = Font::loadFont(path, ptsize, uuid);
      am.add(font, name);
    }
  }
}

} // namespace farixEngine
