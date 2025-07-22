
#include "farixEngine/core/world.hpp"

#include "farixEngine/components/components.hpp"
#include "farixEngine/ecs/system.hpp"
#include "farixEngine/script/script.hpp"
#include "farixEngine/serialization/serializer.hpp"
#include "farixEngine/thirdparty/nlohmann/json.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace farixEngine {
World::World(){
    registerDefaults();
}
World::Entity World::createEntity() {
  Entity id = _nextEntity++;
  entities.push_back(id);
  return id;
}
const std::vector<Entity> &World::getEntities() const{
  return entities;
}
void World::clearStorages() {
  componentManager.clearStorages();
  entities.clear();
  _nextEntity = 1;
  _cameraE = 0;
}
 
void World::setCameraEntity(Entity c) { _cameraE = c; }

World::Entity World::getCamera() const { return _cameraE; }

std::vector<World::Entity>
World::getEntitiesByName(const std::string &name) const {
  std::vector<Entity> result;
  for (Entity e : entities) {
    if (getEntityName(e) == name) {
      result.push_back(e);
    }
  }
  return result;
} 

std::string World::getEntityName(Entity entity) const {
  if (!hasComponent<Metadata>(entity))
    return "";
  return getComponent<Metadata>(entity).name;
}

void World::updateSystems(float dt) { systemManager.updateAll(*this, dt); }

void World::startSystems() { systemManager.startAll(*this); }

void World::addSystem(std::shared_ptr<System> system) {
  systemManager.addSystem(std::move(system));
}

void World::addScript(uint32_t entity, ScriptPtr script) {
  ScriptComponent sc;
  script->setContext(entity, this);
  sc.script = script;
  addComponent(entity, sc);
}
 
void World::setName(Entity entity, const std::string &name) {
  if (!hasComponent<Metadata>(entity)) {
    addComponent(entity, Metadata{});
  }
  getComponent<Metadata>(entity).name = name;
}

void World::addTag(Entity entity, const std::string &tag) {
  if (!hasComponent<Metadata>(entity)) {
    addComponent(entity, Metadata{});
  }
  auto &tags = getComponent<Metadata>(entity).tags;
  if (std::find(tags.begin(), tags.end(), tag) == tags.end())
    tags.push_back(tag);
}

void World::removeTag(Entity entity, const std::string &tag) {
  if (!hasComponent<Metadata>(entity)) {
    addComponent(entity, Metadata{});
    return;
  }
  auto &tags = getComponent<Metadata>(entity).tags;
  auto it = std::find(tags.begin(), tags.end(), tag);
  if (it != tags.end())
    tags.erase(it);
}

bool World::hasTag(Entity entity, const std::string &tag) const {
  if (!hasComponent<Metadata>(entity))
    return false;
  const auto &tags = getComponent<Metadata>(entity).tags;
  return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

void World::setParent(Entity child, Entity parent) {
  if (!hasComponent<ChildrenComponent>(parent)) {
    addComponent(parent, ChildrenComponent{});
  }
  auto &children = getComponent<ChildrenComponent>(parent).children;
  if (std::find(children.begin(), children.end(), child) == children.end()) {
    children.push_back(child);
  }
  if (!hasComponent<ParentComponent>(child)) {
    addComponent(child, ParentComponent{parent});
  } else {
    getComponent<ParentComponent>(child).parent = parent;
  }
}

void World::removeParent(Entity child) {
  if (!hasComponent<ParentComponent>(child))
    return;
  Entity parent = getComponent<ParentComponent>(child).parent;
  if (hasComponent<ChildrenComponent>(parent)) {
    auto &children = getComponent<ChildrenComponent>(parent).children;
    children.erase(std::remove(children.begin(), children.end(), child),
                   children.end());
  }
  removeComponent<ParentComponent>(child);
}

void World::removeChild(Entity parent, Entity child) {
  if (!hasComponent<ChildrenComponent>(parent))
    return;
  auto &children = getComponent<ChildrenComponent>(parent).children;
  children.erase(std::remove(children.begin(), children.end(), child),
                 children.end());
  if (hasComponent<ParentComponent>(child) &&
      getComponent<ParentComponent>(child).parent == parent) {
    removeComponent<ParentComponent>(child);
  }
}

void World::removeAllChildren(Entity parent) {
  if (!hasComponent<ChildrenComponent>(parent))
    return;
  auto &children = getComponent<ChildrenComponent>(parent).children;
  for (Entity child : children) {
    if (hasComponent<ParentComponent>(child)) {
      removeComponent<ParentComponent>(child);
    }
  }
  children.clear();
}

std::vector<World::Entity> World::getChildren(Entity parent) const {
  if (hasComponent<ChildrenComponent>(parent)) {
    return getComponent<ChildrenComponent>(parent).children;
  }
  return {};
}

const std::unordered_map<std::string, ComponentSerializer> &
World::getSerializers() const {
  return componentManager.getSerializerRegistry().getAll();
}
 


void World::setContext(EngineContext *_context) { context = _context; }

EngineContext *World::getContext() const { return context; }

void World::registerDefaults() {
  
registerComponent<Metadata>(
    "Metadata",
    [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<Metadata>(e);
        return {
            {"name", comp.name},
            {"tags", comp.tags},
            {"uuid", comp.uuid},
            {"prefab", comp.prefab}
        };
    },
    [](World &world, Entity e, const json &j) {
        Metadata comp;
        comp.name = j.at("name").get<std::string>();
        comp.tags = j.at("tags").get<std::vector<std::string>>();
        if (j.contains("uuid")) comp.uuid = j.at("uuid").get<std::string>();
        if (j.contains("prefab")) comp.prefab = j.at("prefab").get<std::string>();
        world.addComponent<Metadata>(e, comp);
    });


 
  registerComponent<TransformComponent>(
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
        world.addComponent<TransformComponent>(e, comp);
      });

  registerComponent<GlobalTransform>(
      "GlobalTransform",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<GlobalTransform>(e);
        return {{"worldMatrix", comp.worldMatrix}};
      },
      [](World &world, Entity e, const json &j) {
        GlobalTransform comp;
        comp.worldMatrix = j.at("worldMatrix").get<Mat4>();
        world.addComponent<GlobalTransform>(e, comp);
      });

  registerComponent<CameraComponent>(
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
        world.addComponent<CameraComponent>(e, comp);
      });

  registerComponent<CameraControllerComponent>(
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
        world.addComponent<CameraControllerComponent>(e, comp);
      });

  registerComponent<MeshComponent>(
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

        world.addComponent<MeshComponent>(e, comp);
      });

  registerComponent<MaterialComponent>(
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
        world.addComponent<MaterialComponent>(e, comp);
      });

  registerComponent<ScriptComponent>(
      "ScriptComponent",

      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<ScriptComponent>(e);
        return {{"script", comp.script ? comp.script->name : ""}};
      },

      [](World &world, Entity e, const json &j) {
        ScriptComponent comp;
        std::string name = j.at("script").get<std::string>();

        if (!name.empty()) {
          if (!world.scriptRegistry.exists(name)) {
            throw std::runtime_error("Script type '" + name +
                                     "' not registered.");
          }
          world.addScript(e, world.scriptRegistry.create(name));
        }
      });

  registerComponent<ParentComponent>(
      "ParentComponent",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<ParentComponent>(e);
        return {{"parent", comp.parent}};
      },
      [](World &world, Entity e, const json &j) {
        ParentComponent comp;
        comp.parent = j.at("parent").get<Entity>();
        world.addComponent<ParentComponent>(e, comp);
      });

  registerComponent<ChildrenComponent>(
      "ChildrenComponent",
      [](World &world, Entity e) -> json {
        const auto &comp = world.getComponent<ChildrenComponent>(e);
        return {{"children", comp.children}};
      },
      [](World &world, Entity e, const json &j) {
        ChildrenComponent comp;
        comp.children = j.at("children").get<std::vector<Entity>>();
        world.addComponent<ChildrenComponent>(e, comp);
      });
}

} // namespace farixEngine
