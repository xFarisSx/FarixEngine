# ⚙️ Adding a System

Create a new system by inheriting from the `System` class:

```cpp
class System {
public:
  std::string name = "System";

  System(const std::string &systemName) : name(systemName) {}
  
  virtual void start(World &world) = 0;
  virtual void update(World &world, float dt) = 0;
  
  virtual ~System() = default;
};
```
## Example

```cpp
class BlinkSystem : public System {
public:
  BlinkSystem() : System("BlinkSystem") {}

  void start(World& world) override {
    // initialization
  }

  void update(World& world, float dt) override {
    // update logic
  }
};
```

## Registering The System
```cpp
EngineRegistry::get().getSystemRegistry().registerSystem<TSystem>(name);
```
## Adding it to the world
```cpp
gameWorld->addSystem(std::make_shared<TSystem>());
```
