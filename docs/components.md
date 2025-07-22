# 🔩 Adding a Component

To make a new component serializable and usable with scenes, register it like this:

```cpp
registerComponent<YourComponent>(
  "ComponentName",
  [](World& world, Entity e) -> json {
    const auto& c = world.getComponent<YourComponent>(e);
    return { /* serialize fields */ };
  },
  [](World& world, Entity e, const json& data) {
    YourComponent c{/* deserialize fields */};
    world.addComponent(e, c);
  }
);
```

## Example
```cpp
registerComponent<Transform>(
  "Transform",
  [](World& w, Entity e) {
    const auto& t = w.getComponent<Transform>(e);
    return { {"x", t.x}, {"y", t.y} };
  },
  [](World& w, Entity e, const json& j) {
    w.addComponent(e, Transform{ j["x"], j["y"] });
  }
);
```
