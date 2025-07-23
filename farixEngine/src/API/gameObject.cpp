

#include "farixEngine/API/gameObject.hpp"
#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/components/components.hpp"
#include "farixEngine/script/script.hpp"

namespace farixEngine {

GameObject::GameObject(World &w) : world(&w), entity(w.createEntity()) {
  addComponent<TransformComponent>();
  addComponent<Metadata>();
  setName("GameObject");
}

GameObject::GameObject(World &w, Entity existing)
    : world(&w), entity(existing) {}

Entity GameObject::getEntity() const { return entity; }

void GameObject::setMesh(const std::shared_ptr<Mesh> &mesh) {
  if (!isValid())
    return;

  MeshComponent m;
  m.mesh = mesh;
  if (world->hasComponent<MeshComponent>(entity)) {
    world->getComponent<MeshComponent>(entity) = m;
  } else {
    world->addComponent<MeshComponent>(entity, m);
  }
}

void GameObject::setMaterial(const MaterialComponent &material) {
  if (!isValid())
    return;

  if (world->hasComponent<MaterialComponent>(entity)) {
    world->getComponent<MaterialComponent>(entity) = material;
  } else {
    world->addComponent<MaterialComponent>(entity, material);
  }
}

void GameObject::addScript(const std::shared_ptr<Script> &script) {
  if (!isValid())
    return;

  world->addScript(entity, script);
}

void GameObject::setParent(const GameObject &parent) {
  if (!isValid() || !parent.isValid())
    return;
  world->setParent(entity, parent.getEntity());
}

void GameObject::removeParent() {
  if (!isValid())
    return;
  world->removeParent(entity);
}

std::vector<GameObject> GameObject::getChildren() const {
  std::vector<GameObject> result;
  if (!isValid())
    return result;

  std::vector<Entity> children = world->getChildren(entity);
  result.reserve(children.size());
  for (Entity e : children) {
    result.emplace_back(*world, e);
  }
  return result;
}

void GameObject::setName(const std::string &name) {
  if (!isValid())
    return;
  world->setName(entity, name);
}

void GameObject::addTag(const std::string &tag) {
  if (!isValid())
    return;
  world->addTag(entity, tag);
}

void GameObject::removeTag(const std::string &tag) {
  if (!isValid())
    return;
  world->removeTag(entity, tag);
}

bool GameObject::hasTag(const std::string &tag) {
  if (!isValid())
    return false;
  return world->hasTag(entity, tag);
}

} // namespace farixEngine
