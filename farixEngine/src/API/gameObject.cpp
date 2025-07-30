

#include "farixEngine/API/gameObject.hpp"
#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/components/components.hpp"
#include "farixEngine/script/script.hpp"

namespace farixEngine {

// GameObject::GameObject(GameWorld *gw)
//     : gameWorld(gw), entity(gw->getInternalWorld()->createEntity()) {
//}
GameObject::GameObject(GameWorld *gw, Entity existing)
    : gameWorld(gw), entity(existing) {
  addComponent<TransformComponent>();
  addComponent<Metadata>();
  setName("GameObject");
}

Entity GameObject::getEntity() const { return entity; }

void GameObject::setMesh(const std::shared_ptr<Mesh> &mesh) {
  if (!isValid())
    return;

  MeshComponent m;
  m.mesh = mesh;
  if (world()->hasComponent<MeshComponent>(entity)) {
    world()->getComponent<MeshComponent>(entity) = m;
  } else {
    world()->addComponent<MeshComponent>(entity, m);
  }
}
World *GameObject::world() const {
  assert(gameWorld && "GameObject::world() called on invalid GameObject");
  return gameWorld->getInternalWorld();
}
bool GameObject::isValid() const { return world() && entity != 0; }

void GameObject::setMaterial(const MaterialComponent &material) {
  if (!isValid())
    return;

  if (world()->hasComponent<MaterialComponent>(entity)) {
    world()->getComponent<MaterialComponent>(entity) = material;
  } else {
    world()->addComponent<MaterialComponent>(entity, material);
  }
}
void GameObject::removeScriptByName(const std::string &scriptName) {
  ScriptComponent &scriptComp = getComponent<ScriptComponent>();
  auto it = std::remove_if(scriptComp.scripts.begin(), scriptComp.scripts.end(),
                           [&](const std::shared_ptr<Script> &scriptPtr) {
                             return scriptPtr->name == scriptName;
                           });
  it->get()->onDestroy();
  scriptComp.scripts.erase(it, scriptComp.scripts.end());
}
GameWorld *GameObject::getGameWorld() { return gameWorld; }

void GameObject::destroyObject() {
  gameWorld->destroyObject(*this);
  gameWorld = nullptr;
  entity = 0;
}

std::string &GameObject::getName() { return getComponent<Metadata>().name; }
std::vector<std::string> GameObject::getTags() {
  return getComponent<Metadata>().tags;
}
void GameObject::addScript(const std::shared_ptr<Script> &script) {
  if (!isValid())
    return;

  world()->addScript(entity, script);
  script->onCreate(this, gameWorld->getOwningScene());
}

void GameObject::setParent(const GameObject &parent) {
  if (!isValid() || !parent.isValid())
    return;
  world()->setParent(entity, parent.getEntity());
}

void GameObject::removeParent() {
  if (!isValid())
    return;
  world()->removeParent(entity);
}

std::vector<GameObject *> GameObject::getChildren() {
  std::vector<GameObject *> result;
  if (!isValid())
    return result;

  std::vector<Entity> children = world()->getChildren(entity);
  result.reserve(children.size());
  for (Entity e : children) {
    result.push_back(&getGameWorld()->getGameObject(e));
  }
  return result;
}

void GameObject::setName(const std::string &name) {
  if (!isValid())
    return;
  world()->setName(entity, name);
}

void GameObject::addTag(const std::string &tag) {
  if (!isValid())
    return;
  world()->addTag(entity, tag);
}

void GameObject::removeTag(const std::string &tag) {
  if (!isValid())
    return;
  world()->removeTag(entity, tag);
}

bool GameObject::hasTag(const std::string &tag) {
  if (!isValid())
    return false;
  return world()->hasTag(entity, tag);
}

} // namespace farixEngine
