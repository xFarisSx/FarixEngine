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
- Event system (keyboard, collision, custom events...)
- Scripting system (`start()` / `update()` / `onKeyPressed()` / `onCollision()`, etc.)
- GameObject and GameWorld wrappers for convenient entity & component access
- Hierarchy system with `ParentComponent`, `ChildrenComponent`, and `GlobalTransform` to handle parent-child relationships and global matrices
- Easy extension with user-defined components, systems, and scripts
- Scene management and hot reload support
- Prefabs, save/load using JSON serialization

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
- `RigidBodyComponent` — velocity, acceleration, mass, kinematic flag
- `ColliderComponent` — shape (box, sphere, capsule), size, radius, trigger flag
- `VariableComponent` — flexible key-value storage for floats, ints, strings
- `StateComponent` — state machine with current state and transitions
- `LifetimeComponent` — self-destruct timer for entities
- `AudioSourceComponent` — sound path, looping, volume control
- `LightComponent` — point, directional, spot lights with color, intensity, range, spot angle
- `TimersComponent` — named timers supporting repeat and finish states

---

## Systems

- `RenderSystem` — draws all mesh entities using the active camera 
- `ScriptSystem` — calls `start()` once, then `update(dt)` every frame 
- `HierarchySystem` — updates global transforms based on parent-child hierarchy
- `CameraControllerSystem` — basic WASD + mouse camera movement 
- `PhysicsSystem` — integrates velocity and acceleration for rigid bodies
- `CollisionSystem` — detects collisions between colliders (AABB, spheres, capsules planned)
- `StateSystem` — updates entity state machines and manages transitions
- `LifetimeSystem` — removes entities after their lifetime expires
- `AudioSystem` — manages playback of audio sources (basic scaffolding)
- `TimerSystem` — updates multiple named timers per entity, supports repeats and triggers

---

## Extensibility

You can register custom components, systems, and scripts.
The ECS architecture ensures clean integration into the update loop.
- [Components](docs/components.md)
- [Systems](docs/systems.md) (that iterate over ECS components)
- [Scripts](docs/scripts.md) (by inheriting from `Script`)

---

## Scenes and Prefabs

Learn how to define levels and reusable templates with JSON-based scenes and prefabs:
- [Scenes](docs/scenes.md)
- [Prefabs](docs/prefabs.md)

---

## Events

Learn how to create, emit, subscribe to events:
- [Events](docs/events.md)

## Example

- Sample projects available under `examples/` demonstrate engine usage.

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

- 2D and UI rendering layers with components
- Clipping, improved lighting and shadow support
- Audio system enhancements
- Complete physics system with broadphase, narrowphase, collision response
- Centralized AssetManager for textures, meshes, sounds
- Convert software renderer to OpenGL backend
