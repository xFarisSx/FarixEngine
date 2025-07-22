# Farix Engine (WIP)

A lightweight C++17 game engine using SDL2.  
Features ECS, 3D software rendering, OBJ mesh loading, textures, scripting, scene management, and more.

---

## Requirements

- C++17 compiler  
- SDL2 development libraries  

### Fedora:
```sh
sudo dnf install SDL2-devel
```

### Ubuntu:
```sh
sudo apt install libsdl2-dev
```

---

## Build

```sh
make
```

Outputs: 
- `build/engine/libfarixEngine.a`

---

## Install / Uninstall

```sh
sudo make install     # installs to /usr/local/
sudo make uninstall   # removes installed files
```

---

## Core Features

- Entity-Component-System (ECS)
- Software 3D rendering (OBJ, BMP)
- Lighting & texture support
- Scripting system (`start()` / `update()`)
- GameObject and GameWorld wrappers for convenient entity & component access
- Hierarchy system with `ParentComponent`, `ChildrenComponent`, and `GlobalTransform` to handle parent-child relationships and global matrices
- Easy extension with user-defined components, systems, and scripts
- SceneManager, save/load using JSON serialization
- Component registration and storage management

---

## Components

- `TransformComponent` — position, rotation, and scale 
- `GlobalTransform` — global/world matrix calculated via hierarchy system
- `ParentComponent` — reference to parent entity
- `ChildrenComponent` — list of child entities
- `MeshComponent` — holds a shared mesh reference 
- `MaterialComponent` — texture and lighting parameters 
- `CameraComponent` — FOV, aspect ratio, near/far planes 
- `CameraControllerComponent` — WASD QE and mouse. Enables movement control 
- `ScriptComponent` — attaches logic via script classes
- `Metadata` — name, tags, UUID (used in search/prefab)

---

## Systems

- `RenderSystem` — draws all mesh entities using the active camera 
- `ScriptSystem` — calls `start()` once, then `update(dt)` every frame 
- `HierarchySystem` — updates global transforms based on parent-child hierarchy
- `CameraControllerSystem` — basic WASD + mouse camera movement 

---

## Extensibility

The engine allows custom components and systems.
You can register your own types and logic to extend behavior.
The engine's ECS model ensures that user-defined systems can integrate smoothly into the update loop.
- [Components](docs/components.md)
> ℹ️ **Note:** When registering a component, you must provide a serializer and deserializer using `registerComponent`. See the components guide for examples.
- [Systems](docs/systems.md) (that iterate over ECS components)
- [Scripts](docs/scripts.md) (by inheriting from `Script`)

---

## Example

Check examples

---

## Compile Your Game

```sh
g++ -std=c++17 -Wall -I/usr/local/include/farixEngine main.cpp -L/usr/local/lib -lfarixEngine $(sdl2-config --cflags --libs) -o game
```

---

## Makefile Commands

- `make` → build static library  
- `make clean` → remove build files  
- `sudo make install` → install headers + lib  
- `sudo make uninstall` → remove them  

---

## Notes

- Only `.obj` files (no .mtl/normals yet) 
- Only `.bmp` textures supported 
- Materials include lighting factors and optional texture
- Lighting uses simple ambient, diffuse, specular components
- Software rendering only (OpenGL planned) 
- Some OBJ files may fail — loader is experimental 

---

## To Do / Planned

- Prefab system (via UUID + JSON)
- Event system
- OpenGL backend
- Basic editor
- 2D & UI layers
- Audio system
- Physics
- Scaling the project to include other file formats etc.
