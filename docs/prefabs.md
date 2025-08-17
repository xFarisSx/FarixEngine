# Prefabs

Prefabs are reusable templates for entities, defined as standalone JSON files and instantiated into scenes or dynamically at runtime.

## Usage
### Saving
- You can save any gameObject (and its children) as a prefab using:
```cpp
Prefab::save(gameObject, "prefabs/name.json");
```
### instantiating
- To create a new entity from a prefab:
```cpp
auto varName = Prefab::instantiate(gameWorld, "prefabs/name.json");
```
## Structure of a Prefab File

```json
{
  assets: {
    meshes:[],
    textures:[],
    fonts:[],
    materials:[]
  },
  "components": {
    "Metadata": {
      "name": "Player",
      "prefab": "",
      "tags": ["Paddle"],
      "uuid": "d18d9a0f-2af9-4a59-ac71-123f1e9f1362"
    },
    "TransformComponent": {
      "position": { "x": 0, "y": 5, "z": 0 },
      "rotation": { "x": 0, "y": 0, "z": 0 },
      "scale": { "x": 1, "y": 1, "z": 1 }
    },
    "GlobalTransform": {
      "worldMatrix": [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]]
    },
    "MeshComponent": {
      "type": "Box",
      "mesh": "",
      "size": { "x": 2, "y": 0.2, "z": 0.5 },
      "sphereData": { "x": 1, "y": 16, "z": 32 }
    },
    "MaterialComponent": {
      "baseColor": { "x": 1, "y": 1, "z": 1 },
      "ambient": 0.1,
      "specular": 0.5,
      "shininess": 32,
      "useTexture": false,
      "texture": ""
    },
    "ScriptComponent": {
      "scripts": ["PlayerPaddleScript"]
    }
  },
  "id": 2,
  "children": [
    {
      "components": {
        "Metadata": { "name": "ChildVisual", "tags": [], "uuid": "..." },
        "TransformComponent": {
          "position": { "x": 0, "y": 0.5, "z": 0 },
          "rotation": { "x": 0, "y": 0, "z": 0 },
          "scale": { "x": 0.5, "y": 0.5, "z": 0.5 }
        }
        // Other components...
      }
    }
  ]
}
```

