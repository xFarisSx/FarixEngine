#include "farixEngine/core/engineRegistry.hpp"
#include "farixEngine/components/components.hpp"
#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/script/script.hpp"
#include "farixEngine/systems/systems.hpp"

namespace farixEngine {

ScriptRegistry &EngineRegistry::getScriptRegistry() { return scriptRegistry; }

ComponentSerializerRegistry &EngineRegistry::getSerializerRegistry() {
  return serializerRegistry;
}

SystemRegistry &EngineRegistry::getSystemRegistry() { return systemRegistry; }

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
        return {{"mode", comp.mode == CameraProjectionMode::Perspective
                             ? "Perspective"
                             : "Orthographic"},
                {"fov", comp.fov},
                {"aspectRatio", comp.aspectRatio},
                {"nearPlane", comp.nearPlane},
                {"farPlane", comp.farPlane},
                {"orthoLeft", comp.orthoLeft},
                {"orthoRight", comp.orthoRight},
                {"orthoBottom", comp.orthoBottom},
                {"orthoTop", comp.orthoTop},
                {"orthoNear", comp.orthoNear},
                {"orthoFar", comp.orthoFar}};
      },
      [](World &world, Entity e, const json &j) {
        CameraComponent comp;
        std::string modeStr = j.at("mode").get<std::string>();
        comp.mode = (modeStr == "Perspective")
                        ? CameraProjectionMode::Perspective
                        : CameraProjectionMode::Orthographic;
        comp.fov = j.at("fov").get<float>();
        comp.aspectRatio = j.at("aspectRatio").get<float>();
        comp.nearPlane = j.at("nearPlane").get<float>();
        comp.farPlane = j.at("farPlane").get<float>();
        comp.orthoLeft = j.at("orthoLeft").get<float>();
        comp.orthoRight = j.at("orthoRight").get<float>();
        comp.orthoBottom = j.at("orthoBottom").get<float>();
        comp.orthoTop = j.at("orthoTop").get<float>();
        comp.orthoNear = j.at("orthoNear").get<float>();
        comp.orthoFar = j.at("orthoFar").get<float>();

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
        else if (typeStr == "Sprite")
          comp.mesh = Mesh::createQuad(Vec3(size[0], size[1], size[2]));
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
                {"texture", comp.texture ? comp.texture->path : ""}
        ,{"doubleSided", comp.doubleSided}};
      },
      [](World &world, Entity e, const json &j) {
        MaterialComponent comp;
        comp.baseColor = j.at("baseColor").get<Vec3>();
        comp.ambient = j.at("ambient").get<float>();
        comp.specular = j.at("specular").get<float>();
        comp.shininess = j.at("shininess").get<float>();
        comp.useTexture = j.at("useTexture").get<bool>();
        comp.doubleSided = j.at("doubleSided").get<bool>();
        std::string path = j.at("texture").get<std::string>();
        if (!path.empty()) {
          comp.texture = Texture::loadFromBmp(path);
        }
        world.registerComponent<MaterialComponent>();

        world.addComponent<MaterialComponent>(e, comp);
      });

  rg.registerSerializer<Sprite2DComponent>(
      "Sprite2DComponent",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<Sprite2DComponent>(e);
        return {{"color", comp.color},
                {"flipX", comp.flipX},
                {"flipY", comp.flipY},
                {"size", comp.size},
                {"useTexture", comp.useTexture},
                {"texture", comp.texture ? comp.texture->path : ""}};
      },
      [](World &world, Entity e, const json &j) {
        Sprite2DComponent comp;
        comp.color = j.at("color").get<Vec3>();
      comp.flipX = j.at("flipX").get<bool>();
            comp.flipY = j.at("flipY").get<bool>();
      comp.size = j.at("size").get<Vec3>();

        comp.useTexture = j.at("useTexture").get<bool>();
        std::string path = j.at("texture").get<std::string>();
        if (!path.empty()) {
          comp.texture = Texture::loadFromBmp(path);
        }
        world.registerComponent<Sprite2DComponent>();

        world.addComponent<Sprite2DComponent>(e, comp);
      });

  rg.registerSerializer<ScriptComponent>(
      "ScriptComponent",

      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<ScriptComponent>(e);
        json scripts_json = json::array();

        for (const auto &script : comp.scripts) {
          scripts_json.push_back(script ? script->name : "");
        }

        return {{"scripts", scripts_json}};
      },

      [](World &world, Entity e, const json &j) {
        if (!j.contains("scripts"))
          return;

        const auto &scripts_array = j.at("scripts");
        if (!scripts_array.is_array())
          return;

        world.registerComponent<ScriptComponent>();
        for (const auto &script_name_json : scripts_array) {
          std::string name = script_name_json.get<std::string>();
          if (name.empty())
            continue;

          if (!EngineServices::get()
                   .getEngineRegistry()
                   .getScriptRegistry()
                   .exists(name)) {
            throw std::runtime_error("Script type '" + name +
                                     "' not registered.");
          }

          world.addScript(e, EngineServices::get()
                                 .getEngineRegistry()
                                 .getScriptRegistry()
                                 .create(name));
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

  rg.registerSerializer<RigidBodyComponent>(
      "RigidBodyComponent",
      [](World &world, Entity e) -> json {
        const auto &c = world.getComponent<RigidBodyComponent>(e);
        return {{"velocity", c.velocity},
                {"acceleration", c.acceleration},
                {"mass", c.mass},
                {"isKinematic", c.isKinematic}};
      },
      [](World &world, Entity e, const json &j) {
        RigidBodyComponent c;
        c.velocity = j.at("velocity").get<Vec3>();
        c.acceleration = j.at("acceleration").get<Vec3>();
        c.mass = j.at("mass").get<float>();
        c.isKinematic = j.at("isKinematic").get<bool>();
        world.registerComponent<RigidBodyComponent>();
        world.addComponent<RigidBodyComponent>(e, c);
      });

  rg.registerSerializer<ColliderComponent>(
      "ColliderComponent",
      [](World &world, Entity e) -> json {
        const auto &c = world.getComponent<ColliderComponent>(e);
        return {{"shape", static_cast<int>(c.shape)},
                {"size", c.size},
                {"radius", c.radius},
                {"isTrigger", c.isTrigger}};
      },
      [](World &world, Entity e, const json &j) {
        ColliderComponent c;
        c.shape =
            static_cast<ColliderComponent::Shape>(j.at("shape").get<int>());
        c.size = j.at("size").get<Vec3>();
        c.radius = j.at("radius").get<float>();
        c.isTrigger = j.at("isTrigger").get<bool>();
        world.registerComponent<ColliderComponent>();
        world.addComponent<ColliderComponent>(e, c);
      });

  rg.registerSerializer<BillboardComponent>(
      "BillboardComponent",
      [](World &world, Entity e) -> json {
        const auto &c = world.getComponent<BillboardComponent>(e);
        return {{"type", static_cast<int>(c.type)}};
      },
      [](World &world, Entity e, const json &j) {
        BillboardComponent c;
        c.type =
            static_cast<BillboardComponent::BillboardType>(j.at("type").get<int>());

        world.registerComponent<BillboardComponent>();
        world.addComponent<BillboardComponent>(e, c); 
      });


  rg.registerSerializer<VariableComponent>(
      "VariableComponent",
      [](World &world, Entity e) -> json {
        const auto &c = world.getComponent<VariableComponent>(e);
        return {{"floats", c.floats}, {"ints", c.ints}, {"strings", c.strings}};
      },
      [](World &world, Entity e, const json &j) {
        VariableComponent c;
        c.floats = j.at("floats").get<std::unordered_map<std::string, float>>();
        c.ints = j.at("ints").get<std::unordered_map<std::string, int>>();
        c.strings =
            j.at("strings").get<std::unordered_map<std::string, std::string>>();
        world.registerComponent<VariableComponent>();
        world.addComponent<VariableComponent>(e, c);
      });

  rg.registerSerializer<StateComponent>(
      "StateComponent",
      [](World &world, Entity e) -> json {
        const auto &c = world.getComponent<StateComponent>(e);
        return {{"currentState", c.currentState},
                {"transitions", c.transitions}};
      },
      [](World &world, Entity e, const json &j) {
        StateComponent c;
        c.currentState = j.at("currentState").get<std::string>();
        c.transitions =
            j.at("transitions")
                .get<std::unordered_map<std::string, std::string>>();
        world.registerComponent<StateComponent>();
        world.addComponent<StateComponent>(e, c);
      });

  rg.registerSerializer<LifetimeComponent>(
      "LifetimeComponent",
      [](World &world, Entity e) -> json {
        const auto &c = world.getComponent<LifetimeComponent>(e);
        return {{"timeRemaining", c.timeRemaining}};
      },
      [](World &world, Entity e, const json &j) {
        LifetimeComponent c;
        c.timeRemaining = j.at("timeRemaining").get<float>();
        world.registerComponent<LifetimeComponent>();
        world.addComponent<LifetimeComponent>(e, c);
      });

  rg.registerSerializer<AudioSourceComponent>(
      "AudioSourceComponent",
      [](World &world, Entity e) -> json {
        const auto &c = world.getComponent<AudioSourceComponent>(e);
        return {
            {"soundPath", c.soundPath}, {"loop", c.loop}, {"volume", c.volume}};
      },
      [](World &world, Entity e, const json &j) {
        AudioSourceComponent c;
        c.soundPath = j.at("soundPath").get<std::string>();
        c.loop = j.at("loop").get<bool>();
        c.volume = j.at("volume").get<float>();
        world.registerComponent<AudioSourceComponent>();
        world.addComponent<AudioSourceComponent>(e, c);
      });

  rg.registerSerializer<LightComponent>(
      "LightComponent",
      [](World &world, Entity e) -> json {
        const auto &c = world.getComponent<LightComponent>(e);
        return {{"type", static_cast<int>(c.type)},
                {"color", {c.color.x, c.color.y, c.color.z}},
                {"dir", {c.dir.x, c.dir.y, c.dir.z}},
                {"intensity", c.intensity},
                {"range", c.range},
                {"spotAngle", c.spotAngle}};
      },
      [](World &world, Entity e, const json &j) {
        LightComponent c;
        c.type = static_cast<LightComponent::Light>(j.at("type").get<int>());
        auto col = j.at("color");
        c.color = Vec3(col[0], col[1], col[2]);
        auto dir = j.at("dir");
        c.dir = Vec3(dir[0], dir[1], dir[2]);
        c.intensity = j.at("intensity").get<float>();
        c.range = j.at("range").get<float>();
        c.spotAngle = j.at("spotAngle").get<float>();
        world.registerComponent<LightComponent>();
        world.addComponent<LightComponent>(e, c);
      });

  rg.registerSerializer<TimersComponent>(
      "TimersComponent",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<TimersComponent>(e);
        json timersJson = json::object();

        for (const auto &[name, timer] : comp.timers) {
          timersJson[name] = {{"current", timer->current},
                              {"max", timer->max},
                              {"repeat", timer->repeat},
                              {"finished", timer->finished},
                              {"name", timer->name}};
        }

        return timersJson;
      },
      [](World &world, Entity e, const json &j) {
        TimersComponent comp;
        for (auto it = j.begin(); it != j.end(); ++it) {
          auto name = it.key();
          auto jt = it.value();
          auto timer = std::make_shared<Timer>();
          timer->current = jt.at("current").get<float>();
          timer->max = jt.at("max").get<float>();
          timer->repeat = jt.at("repeat").get<bool>();
          timer->finished = jt.at("finished").get<bool>();
          timer->name = jt.at("name").get<std::string>();
          comp.timers[name] = timer;
        }
        world.registerComponent<TimersComponent>();
        world.addComponent<TimersComponent>(e, comp);
      });
}

} // namespace farixEngine
