

#pragma once

#include "farixEngine/assets/material.hpp"
#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/components/components.hpp"
#include "farixEngine/core/world.hpp"

#include <utility>

namespace farixEngine {
class GameWorld;
class GameObject {
  World *world = nullptr;
  Entity entity = 0;

public:
  GameObject() = default;
  GameObject(World &w);
  GameObject(World &w, Entity existing);

  Entity getEntity() const;
  bool isValid() const { return world && entity != 0; }

  template <typename T> GameObject &addComponent(const T &component);
  template <typename T> GameObject &addComponent();
  template <typename T, typename... Args,
            typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  GameObject &emplaceComponent(Args &&...args);
  template <typename... T> GameObject &addComponents();
  template <typename... T, typename... Components>
  GameObject &addComponents(Components &&...components);

  template <typename T> T &getComponent();
  template <typename T> bool hasComponent();
  template <typename T> void removeComponent();

  void setMesh(const std::shared_ptr<Mesh> &mesh);
  void setMaterial(const MaterialComponent &material);
  void addScript(const std::shared_ptr<Script> &script);

  void setParent(const GameObject &parent);
  void removeParent();
  std::vector<GameObject> getChildren() const;
 
  void setName(const std::string &name);
  void addTag(const std::string &tag);
  void removeTag(const std::string &tag);
  bool hasTag(const std::string &tag);
  std::string& getName();
  std::vector<std::string> getTags();

  GameWorld getGameWorld();

  template <typename... Tags> void addTags(Tags &&...tags);
  template <typename... Tags> void removeTags(Tags &&...tags);
  template <typename... Tags> bool hasTags(Tags &&...tags);

  void removeScriptByName(const std::string &scriptName) ;

  void destroyObject();

};

template <typename T> GameObject &GameObject::addComponent(const T &component) {
  world->addComponent<T>(entity, component);
  return *this;
}
template <typename... T> GameObject &GameObject::addComponents() {
  (world->addComponent<T>(entity), ...);
  return *this;
}
template <typename... T, typename... Components>
GameObject &GameObject::addComponents(Components &&...components) {
  (world->addComponent<T>(entity, std::forward<Components>(components)), ...);
  return *this;
}
template <typename T> GameObject &GameObject::addComponent() {
  world->addComponent<T>(entity);
  return *this;
}
template <typename T, typename... Args, typename>
GameObject &GameObject::emplaceComponent(Args &&...args) {
  T component(std::forward<Args>(args)...);
  world->addComponent<T>(entity, component);
  return *this;
}

template <typename T> T &GameObject::getComponent() {
  return world->getComponent<T>(entity);
}

template <typename T> bool GameObject::hasComponent() {
  return world->hasComponent<T>(entity);
}

template <typename T> void GameObject::removeComponent() {
  world->removeComponent<T>(entity);
}

template <typename... Tags> void GameObject::addTags(Tags &&...tags) {
  (addTag(std::forward<Tags>(tags)), ...);
}

template <typename... Tags> void GameObject::removeTags(Tags &&...tags) {
  (removeTag(std::forward<Tags>(tags)), ...);
}

template <typename... Tags> bool GameObject::hasTags(Tags &&...tags) {
  return (hasTag(std::forward<Tags>(tags)) && ...);
}

} // namespace farixEngine
