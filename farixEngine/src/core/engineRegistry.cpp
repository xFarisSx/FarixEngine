#include "farixEngine/core/engineRegistry.hpp"
#include "farixEngine/components/components.hpp"
#include "farixEngine/script/script.hpp"

namespace farixEngine {

EngineRegistry &EngineRegistry::get() {
  static EngineRegistry instance;
  return instance;
}

ScriptRegistry &EngineRegistry::getScriptRegistry() { return scriptRegistry; }

ComponentSerializerRegistry &EngineRegistry::getSerializerRegistry() {
  return serializerRegistry;
}

void EngineRegistry::clear() {}

void EngineRegistry::registerDefaults() {
  ComponentSerializerRegistry &rg = getSerializerRegistry();

  rg.registerSerializer<Metadata>(
      "Metadata",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<Metadata>(e);
        return {{"name", comp.name},
                {"tags", comp.tags},
                {"uuid", comp.uuid},
                {"prefab", comp.prefab}};
      },
      [](World &world, Entity e, const json &j) {
        Metadata comp;
        comp.name = j.at("name").get<std::string>();
        comp.tags = j.at("tags").get<std::vector<std::string>>();
        if (j.contains("uuid"))
          comp.uuid = j.at("uuid").get<std::string>();
        if (j.contains("prefab"))
          comp.prefab = j.at("prefab").get<std::string>();
      world.registerComponent<Metadata>();
        world.addComponent<Metadata>(e, comp);
      });

  rg.registerSerializer<TransformComponent>(
      "TransformComponent",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<TransformComponent>(e);
        return {{"position", comp.position},
                {"rotation", comp.rotation},
                {"scale", comp.scale}};
      },
      [](World &world, Entity e, const json &j) {
        TransformComponent comp;
        comp.position = j.at("position").get<Vec3>();
        comp.rotation = j.at("rotation").get<Vec3>();
        comp.scale = j.at("scale").get<Vec3>();
            world.registerComponent<TransformComponent>();

        world.addComponent<TransformComponent>(e, comp);
      });

  rg.registerSerializer<GlobalTransform>(
      "GlobalTransform",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<GlobalTransform>(e);
        return {{"worldMatrix", comp.worldMatrix}};
      },
      [](World &world, Entity e, const json &j) {
        GlobalTransform comp;
        comp.worldMatrix = j.at("worldMatrix").get<Mat4>();

        world.registerComponent<GlobalTransform>();

        world.addComponent<GlobalTransform>(e, comp);
      });

  rg.registerSerializer<CameraComponent>(
      "CameraComponent",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<CameraComponent>(e);
        return {{"fov", comp.fov},
                {"aspectRatio", comp.aspectRatio},
                {"nearPlane", comp.nearPlane},
                {"farPlane", comp.farPlane}};
      },
      [](World &world, Entity e, const json &j) {
        CameraComponent comp;
        comp.fov = j.at("fov").get<float>();
        comp.aspectRatio = j.at("aspectRatio").get<float>();
        comp.nearPlane = j.at("nearPlane").get<float>();
        comp.farPlane = j.at("farPlane").get<float>();
      world.registerComponent<CameraComponent>();
        world.addComponent<CameraComponent>(e, comp);
      });

  rg.registerSerializer<CameraControllerComponent>(
      "CameraControllerComponent",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<CameraControllerComponent>(e);
        return {{"sens", comp.sens},
                {"speed", comp.speed},
                {"active", comp.active}};
      },
      [](World &world, Entity e, const json &j) {
        CameraControllerComponent comp;
        comp.sens = j.at("sens").get<float>();
        comp.speed = j.at("speed").get<float>();
        comp.active = j.at("active").get<bool>();
            world.registerComponent<CameraControllerComponent>();
        world.addComponent<CameraControllerComponent>(e, comp);
      });

  rg.registerSerializer<MeshComponent>(
      "MeshComponent",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<MeshComponent>(e);
        return {{"mesh", comp.mesh ? comp.mesh->path : ""},
                {"type", comp.mesh ? comp.mesh->type : "None"},
                {"size", comp.mesh ? comp.mesh->size : Vec3(1)},
                {"sphereData",
                 comp.mesh ? comp.mesh->sphereData : Vec3{1, 16, 32}}};
      },
      [](World &world, Entity e, const json &j) {
        MeshComponent comp;
        std::string path = j.value("mesh", "");
        std::string typeStr = j.value("type", "None");
        Vec3 size = j.value("size", Vec3(1));
        Vec3 sd = j.value("sphereData", Vec3(1));

        if (typeStr == "Obj")
          comp.mesh = Mesh::loadFromObj(path);
        else if (typeStr == "Box")
          comp.mesh = Mesh::createBox(size[0], size[1], size[2]);
        else if (typeStr == "Sphere")
          comp.mesh = Mesh::createSphere(sd[0], sd[1], sd[2]);
         world.registerComponent<MeshComponent>();

        world.addComponent<MeshComponent>(e, comp);
      });

  rg.registerSerializer<MaterialComponent>(
      "MaterialComponent",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<MaterialComponent>(e);
        return {{"baseColor", comp.baseColor},
                {"ambient", comp.ambient},
                {"specular", comp.specular},
                {"shininess", comp.shininess},
                {"useTexture", comp.useTexture},
                {"texture", comp.texture ? comp.texture->path : ""}};
      },
      [](World &world, Entity e, const json &j) {
        MaterialComponent comp;
        comp.baseColor = j.at("baseColor").get<Vec3>();
        comp.ambient = j.at("ambient").get<float>();
        comp.specular = j.at("specular").get<float>();
        comp.shininess = j.at("shininess").get<float>();
        comp.useTexture = j.at("useTexture").get<bool>();
        std::string path = j.at("texture").get<std::string>();
        if (!path.empty()) {
          comp.texture = Texture::loadFromBmp(path);
        }
      world.registerComponent<MaterialComponent>();

        world.addComponent<MaterialComponent>(e, comp);
      });

  rg.registerSerializer<ScriptComponent>(
      "ScriptComponent",

      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<ScriptComponent>(e);
        return {{"script", comp.script ? comp.script->name : ""}};
      },

      [](World &world, Entity e, const json &j) {
        ScriptComponent comp;
        std::string name = j.at("script").get<std::string>();

        if (!name.empty()) {
          if (!EngineRegistry::get().getScriptRegistry().exists(name)) {
            throw std::runtime_error("Script type '" + name +
                                     "' not registered.");
          }
        world.registerComponent<ScriptComponent>();

          world.addScript(
              e, EngineRegistry::get().getScriptRegistry().create(name));
        }
      });

  rg.registerSerializer<ParentComponent>(
      "ParentComponent",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<ParentComponent>(e);
        return {{"parent", comp.parent}};
      },
      [](World &world, Entity e, const json &j) {
        ParentComponent comp;
        comp.parent = j.at("parent").get<Entity>();
      world.registerComponent<ParentComponent>();

        world.addComponent<ParentComponent>(e, comp);
      });

  rg.registerSerializer<ChildrenComponent>(
      "ChildrenComponent",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<ChildrenComponent>(e);
        return {{"children", comp.children}};
      },
      [](World &world, Entity e, const json &j) {
        ChildrenComponent comp;
        comp.children = j.at("children").get<std::vector<Entity>>();
      world.registerComponent<ChildrenComponent>();

        world.addComponent<ChildrenComponent>(e, comp);
      });
}

} // namespace farixEngine
