# Farix Engine (WIP)

A lightweight C++17 game engine using SDL2 and OpenGL.
Supports ECS, 3D/2.5D/2D rendering, OBJ mesh loading, textures, scripting, scene management, UI, and more.

---

## Requirements

- C++17 compiler
- SDL2 development libraries
- SDL2_ttf development libraries
- OpenGL

### Linux:
```sh
# Fedora
sudo dnf install SDL2-devel SDL2_ttf-devel mesa-libGL-devel

# Ubuntu
sudo apt install libsdl2-dev libsdl2-ttf-dev libgl1-mesa-dev
```

### Windows (Visual Studio + vcpkg):
- Install `vcpkg`
```sh
vcpkg install sdl2 sdl2-ttf glew
```
---

## Build and Install Engine (CMake)
### Linux:
```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
sudo cmake --install .        # installs headers, library, and assets
```
### Windows (Visual Studio + vcpkg):
```sh
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/FarixEngine"
cmake --build . --config Release
cmake --install . --config Release
```
- Include FarixEngine folder in your project include paths.

---

## Core Features

- Entity-Component-System (ECS)
- Responsive Software and OpenGL backend
- Lighting & texture support
- 2D rendering (sprites, UI) and 3D rendering
- UI system: anchors, images, texts, buttons
- Event system (keyboard, collision, custom events...)
- Scripting system (`onStart()` / `onUpdate()` / `onKeyPressed()` / `onCollision()`, etc.)
- GameObject and GameWorld wrappers for convenient entity & component access
- Centralized asset manager
- Hierarchy system with parenting and global transforms
- Easy extension with user-defined components, systems, and scripts
- Scene management and hot reload support
- Prefabs, save/load using JSON serialization

---

## Components
### Core
- `TransformComponent` — position, rotation, and scale 
- `GlobalTransform` — global/world matrix calculated via hierarchy system
- `Metadata` — name, tags, UUID (used in search/prefab)
- `ParentComponent` — reference to parent entity
- `ChildrenComponent` — list of child entities
### 3D Rendering
- `MeshComponent` — holds a shared mesh reference 
- `MaterialComponent` — texture and lighting parameters 
- `BillboardComponent` — face camera (Y-only/full)
### Cameras
- `CameraComponent` — FOV, aspect ratio, near/far planes 
- `CameraControllerComponent` — WASD QE and mouse. Enables movement control 
### Scripting
- `ScriptComponent` — attaches logic via script classes
- `VariableComponent` — flexible key-value storage for floats, ints, strings
- `StateComponent` — state machine with current state and transitions
### Physics & Collision
- `RigidBodyComponent` — velocity, acceleration, mass, kinematic flag
- `ColliderComponent` — shape (box, sphere, capsule), size, radius, trigger flag

### Lifecycle
- `LifetimeComponent` — self-destruct timer for entities
- `TimersComponent` — named timers supporting repeat and finish states
### Audio
- `AudioSourceComponent` — sound path, looping, volume control
### Lighting
- `LightComponent` — point, directional, spot lights with color, intensity, range, spot angle
### 2D & UI Components
- `Sprite2DComponent` — textured quad with color tint
- `TextComponent` — 3D world-space text
- `RectComponent` — screen-space position, size, rotation
- `UIComponent` — anchor (top/center/etc.), interactivity flags
- `UIImageComponent` — draws texture to screen (UI)
- `UITextComponent` — text string, font, size, color
- `UIButtonComponent` — clickable logic with hover/press state

---

## Systems

- `RenderSystem` — draws 3D/2D entities
- `ScriptSystem` — calls `onStart()` once, then `onUpdate(dt)` every frame 
- `HierarchySystem` — updates global transforms based on parent-child hierarchy
- `CameraControllerSystem` — basic WASD + mouse camera movement 
- `BillboardSystem` — faces mesh/quads toward camera
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

### CMake For A Game (MyGame)
```cmake
cmake_minimum_required(VERSION 3.15)
project(MyGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{HOME}/Builds/MyGameBuild)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenGL REQUIRED)
find_package(FarixEngine REQUIRED)

add_executable(mygame
    main.cpp
    game/game.cpp
)

target_link_libraries(mygame
    PRIVATE
        Farix::farixEngine
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_ttf::SDL2_ttf
        OpenGL::GL
)

```

---

## Manual Compilation
### Linux:

```sh
g++ -std=c++17 -Wall \
    main.cpp \
    game/game.cpp \
    -I/usr/local/include/farixEngine \
    -L/usr/local/lib -lfarixEngine \
    $(sdl2-config --cflags --libs) -lSDL2_ttf -o ~/Builds/CppProjectBuild/game

```
### Windows (MinGW g++):
- Assuming `SDL2`, `SDL2_ttf` and `OpenGL` are installed (via vcpkg or manually)
```sh
g++ -std=c++17 -Wall \
    main.cpp \
    game/game.cpp \
    -IC:/path/to/farixEngine/include \
    -LC:/path/to/farixEngine/lib -lfarixEngine \
    -IC:/path/to/SDL2/include -LC:/path/to/SDL2/lib -lSDL2 -lSDL2_ttf -lopengl32 \
    -o game.exe
```

---

## Notes

- Only `.obj` files (no .glTF yet) 
- Materials include lighting factors and optional texture
- Lighting: simple Phong-like components

---

## To Do / Planned

- Rendering enhancements (shadows, post-processing)
- Audio system enhancements
- Complete physics (broadphase/narrowphase, collision response)
- Scale the engine to include various types and formats of assets like glTF etc.
- More sample projects
