# ⚙️ Adding a System

Create a new system by inheriting from the `System` class:

```cpp
class MySystem : public System {
public:
  void start(World& world) override {
    // optional setup
  }

  void update(World& world, float dt) override {
    // logic here
  }
};
```

## Registering The System
```cpp
world.addSystem(std::make_shared<MySystem>());
```
