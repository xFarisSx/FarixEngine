# Scripts

Scripts allow you to define per-entity behaviors, similar to scripting in Unity or Godot.

To create a script:
- Inherit from the `Script` base class.
- Override `start()` and `update(float dt)` methods.
- Use `getComponent<T>()` to access components on the same entity.

## Script API

```cpp
class Script {
protected:
  uint32_t entityId = 0;
  World* world = nullptr;

public:
  std::string name = "Script";
  Script(const std::string& scriptName);
  virtual ~Script() = default;

  virtual void start();          // Called once when the script starts
  virtual void update(float dt); // Called every frame

  void setContext(uint32_t id, World* w); // Sets context internally

  template <typename T>
  T& getComponent(); // Access a component on the same entity
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

- Register the script class using `registerScript<T>()`
- Add it to an entity using `addScript(entity, scriptInstance)`

```cpp
world.registerScript<Rotator>("Rotator"); // Registers the script under a name
world.addScript(modelEntity, std::make_shared<Rotator>()); // Attaches the script to the entity
```
- Note: You can register multiple scripts with different names. At runtime, only registered scripts can be instantiated by name (e.g., from scene files).

- Tip: Script objects are stored via std::shared_ptr<Script>, and automatically managed by the ECS engine.



