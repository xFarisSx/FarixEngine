# Scripts

Scripts allow you to define per-entity behaviors, similar to scripting in Unity or Godot.

To create a script:
- Inherit from the `Script` base class.
- Override `start()` and `update(float dt)` methods.
- Use `getComponent<T>()` to access components on the same entity.
- Use `getGameWorld()` and `getGameObject()` to access context.

## Script API

```cpp
class Script {
  friend class ScriptSystem;
private:
  uint32_t entityId = 0;
  World *world = nullptr;
  bool started = false;


public:
  std::string name = "Script";

  Script(const std::string &scriptName) : name(scriptName) {}

  virtual ~Script() = default;

  virtual void start() {}

  virtual void update(float dt) {}

  void setContext(uint32_t id, World *w);

  template <typename T> T &getComponent();
  GameObject getGameObject();
  GameWorld getGameWorld();
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




