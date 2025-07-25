# 🔩 Adding a Component

## Local registration (No serializing)

```cpp
world/gameWorld.registerComponent<Type>();
```

## Serialization registration (for scenes and prefabs)

```cpp
EngineServices::get().getEngineRegistry().getSerializerRegistry().registerSerializer<YourComponent>(
  "ComponentName",
  [](World& world, Entity e) -> json {
    const auto& c = world.getComponent<YourComponent>(e);
    return { /* serialize fields */ };
  },
  [](World& world, Entity e, const json& data) {
    YourComponent c{/* deserialize fields */};
    check if not registered
    world.addComponent(e, c);
  }
);
```

### Example
```cpp
EngineServices::get().getEngineRegistry().getSerializerRegistry().registerSerializer<BlinkComponent>(
  "BlinkComponent",
  [](World &world, Entity e) -> json {
    const auto &blink = world.getComponent<BlinkComponent>(e);
    return json{{"timer", blink.timer}, {"interval", blink.interval}, {"visible", blink.visible}};
  },
  [](World &world, Entity e, const json &j) {
    BlinkComponent blink;
    blink.timer = j.value("timer", 0.0f);
    blink.interval = j.value("interval", 0.5f);
    blink.visible = j.value("visible", true);
    world.addComponent(e, blink);
  }
);
```
