# Scripts

Scripts allow you to define per-entity behaviors, similar to scripting in Unity or Godot.

To create a script:
- Inherit from the `Script` base class.
- Override `onStart()` and `onUpdate(float dt)` and optionally the events methods.
- Use `getComponent<T>()` to access components on the same entity.
- Use `getGameWorld()` and `getGameObject()` and `getScene()` to access context.

## Script API

```cpp
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
  
  virtual void onCollision(CollisionEvent &collider){}
  virtual void onKeyPressed(KeyPressedEvent &event) {}
  virtual void onKeyReleased(KeyReleasedEvent &event) {}
  
  virtual void onCreate(GameObject *obj, Scene *scene) {} // You should call it from Super before modifying it
  virtual void onDestroy() {} // You should call it from Super before modifying it

  void setContext(uint32_t id, World *w);

  template <typename T> T &getComponent();
  GameObject *getGameObject();
  GameWorld *getGameWorld();
  Scene *getScene() const;
};
```

## Example
```cpp
class Rotator : public Script {
public:
  Rotator() : Script("Rotator") {}

  void update(float dt) override {
    auto& transform = getComponent<Transform>();
    transform.rotation.y += 90.0f * dt;
  }
};
```

## Registering and Adding Scripts

To use a script at runtime, you must:

- Register the script class using `EngineServices::get().getEngineRegistry().getScriptRegistry().registerScript<T>(name);`
- Add it to a gameObject using `gameObject.addScript(std::make_shared<TScript>());`
- Remove by name using `gameObject.removeScriptByName(name);`




