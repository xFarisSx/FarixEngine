# Scenes

Farix Engine uses JSON-based scene files to define and load collections of entities, components, and active systems. Scenes are managed by the SceneManager and executed through the Scene class.

## Scene JSON Structure

```json
{
  "name": "sceneName",
  "activeCamera": <entityId>,
  "entities": [ /* array of serialized entities */ ],
  "systems": [ /* list of system names to activate */ ]
}
```
### Entities Array
Each entry:
```json
{
  "id": <uint>,
  "components": {
    "Transform": { /* ... */ },
    "Mesh": { /* ... */ },
    // etc.
  }
}
```
### Systems Array
- A list of registered system names to re‑add on load.

## Example
```cpp
SceneManager &sceneManager = getSceneManager();

// Create scene and register components
auto &scene = sceneManager.createScene("pong");
scene.gameWorld().registerComponent<BlinkComponent>();

// Setup scene
setupScene(*sceneManager.currentGameWorld());

// Add systems
sceneManager.currentGameWorld()->addSystem(std::make_shared<BlinkSystem>());

// Save to disk
sceneManager.saveCurrentScene("scenes/");
```
