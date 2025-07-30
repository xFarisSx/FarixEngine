#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "farixEngine/API/gameObject.hpp"
#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/core/world.hpp"
#include "farixEngine/events/eventDispatcher.hpp"
#include "farixEngine/events/events.hpp"
#include "farixEngine/scene/scene.hpp"
namespace farixEngine {
class Scene;
class Script {
  friend class ScriptSystem;

private:
  bool started = false;
  EventDispatcher::ListenerID collisionListener = 0;
  EventDispatcher::ListenerID keypListener = 0;
  EventDispatcher::ListenerID keyrListener = 0;

protected:
  Scene *scene = nullptr;
  Entity entity = 0;
  World *world = nullptr;
  GameObject *gameObject = nullptr;

public:
  std::string name = "Script";

  Script(const std::string &scriptName) : name(scriptName) {}

  virtual ~Script() { onDestroy(); }

  virtual void onStart() {}

  virtual void onUpdate(float dt) {}
  virtual void onCreate(GameObject *obj, Scene *scene) {
    this->gameObject = obj;
    this->scene = scene;

    collisionListener = scene->getEventDispatcher().listen<CollisionEvent>(
        [this](CollisionEvent &event) {
          if (event.a->getEntity() == entity ||
              event.b->getEntity() == entity) {
            this->onCollision(event);
          }
        });

    keypListener = scene->getEventDispatcher().listen<KeyPressedEvent>(
        [this](KeyPressedEvent &e) { onKeyPressed(e); });

    keyrListener = scene->getEventDispatcher().listen<KeyReleasedEvent>(
        [this](KeyReleasedEvent &e) { onKeyReleased(e); });
  }
  virtual void onCollision(CollisionEvent &collider) {}
  virtual void onKeyPressed(KeyPressedEvent &event) {}
  virtual void onKeyReleased(KeyReleasedEvent &event) {}

  virtual void onDestroy() {
    EngineServices::get().getEventDispatcher().removeListener<CollisionEvent>(
        collisionListener);
    EngineServices::get().getEventDispatcher().removeListener<KeyPressedEvent>(
        keypListener);
    EngineServices::get().getEventDispatcher().removeListener<KeyReleasedEvent>(
        keyrListener);
    gameObject = nullptr;
    scene = nullptr;
    world = nullptr;
    entity = 0;
  }

  void setContext(Entity e, World *w) {
    world = w;
    entity = e;
  }

  template <typename T> T &getComponent();
  GameObject *getGameObject();
  GameWorld *getGameWorld();
  Scene *getScene() const;
};

template <typename T> T &Script::getComponent() {
  return getGameObject()->getComponent<T>();
}

} // namespace farixEngine
