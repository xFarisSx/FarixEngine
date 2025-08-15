#pragma once
#include "farixEngine/components/components.hpp"
#include "farixEngine/ecs/system.hpp"
#include "farixEngine/input/controller.hpp"
#include "farixEngine/renderer/renderData.hpp"
#include "farixEngine/renderer/renderer.hpp"
#include <memory>

namespace farixEngine {
class RenderSystem : public System {

public:
  RenderSystem() : System("RenderSystem") {}

  void onStart(World &world) override {};

  void onUpdate(World &world, float dt) override;

  Mat4 getViewMatrix(World &world);
  Mat4 getProjectionMatrix(World &world);
  Vec3 calculateAnchoredPosition(const RectComponent &rect,
                                 UIComponent::Anchor anchor, float screenWidth,
                                 float screenHeight);

  renderer::RenderContext createRenderContext(World &world,
                                              const CameraComponent &cam,
                                              const Mat4 &cameraTransform,
                                              const Vec3 &camPosition);
  renderer::MeshData loadMeshFromAsset(AssetID id);
  renderer::MeshData &createOrGetMesh(AssetID id);
  renderer::MaterialData &createOrGetMaterial(AssetID id);
  void applyMaterialOverrides(renderer::MaterialData& matData ,MaterialOverrides& overrides);

private:
  std::unordered_map<AssetID, renderer::MeshData> meshCache;
  std::unordered_map<AssetID, renderer::MaterialData> materialCache;
};

class ScriptSystem : public System {
public:
  ScriptSystem() : System("ScriptSystem") {}

  void onStart(World &world) override;

  void onUpdate(World &world, float dt) override;
};

class HierarchySystem : public System {
public:
  HierarchySystem() : System("HierarchySystem") {}

  void onStart(World &world) override {};

  void onUpdate(World &world, float dt) override;
};

class CameraControllerSystem : public System {
public:
  CameraControllerSystem() : System("CameraControlSystem") {}

  void onStart(World &world) override {};

  void onUpdate(World &world, float dt) override;
};

class PhysicsSystem : public System {
public:
  PhysicsSystem() : System("PhysicsSystem") {}

  void onStart(World &world) override {}

  void onUpdate(World &world, float dt) override;
};

class CollisionSystem : public System {
public:
  CollisionSystem() : System("CollisionSystem") {}

  void onStart(World &world) override {}

  void onUpdate(World &world, float dt) override;
};

class StateSystem : public System {
public:
  StateSystem() : System("StateSystem") {}

  void onStart(World &world) override {}

  void onUpdate(World &world, float dt) override;
};

class LifetimeSystem : public System {
public:
  LifetimeSystem() : System("LifetimeSystem") {}

  void onStart(World &world) override {}

  void onUpdate(World &world, float dt) override;
};

class AudioSystem : public System {
public:
  AudioSystem() : System("AudioSystem") {}

  void onStart(World &world) override {}
  void onUpdate(World &world, float dt) override;
};

class TimerSystem : public System {
public:
  TimerSystem() : System("TimerSystem") {}

  void onStart(World &world) override {}

  void onUpdate(World &world, float dt) override;

  void addTimer(World &world, Entity e, const std::string &name, float max,
                bool repeat);
  void removeTimer(World &world, Entity e, const std::string &name);
};

class BillboardSystem : public System {
public:
  BillboardSystem() : System("BillboardSystem") {}

  void onStart(World &world) override {}
  void onUpdate(World &world, float dt) override;
};

} // namespace farixEngine
