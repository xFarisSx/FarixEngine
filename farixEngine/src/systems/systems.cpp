#include "farixEngine/systems/systems.hpp"
#include "farixEngine/physics/collisionHelpers.hpp"
#include "farixEngine/components/components.hpp"
#include "farixEngine/core/world.hpp"
#include "farixEngine/ecs/system.hpp"
#include "farixEngine/math/general.hpp"
#include "farixEngine/math/mat4.hpp"
#include "farixEngine/renderer/renderer.hpp"
#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/script/script.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
namespace farixEngine {
using Entity = uint32_t;

void RenderSystem::update(World &world, float dt) {
  Renderer* renderer = EngineServices::get().getContext()->renderer;
  const auto &entities = world.getEntities();

  Entity cameraEntity = world.getCamera();

  if (!cameraEntity || cameraEntity <= 0)
    return;

  auto &cameraGM =
      world.getComponent<GlobalTransform>(cameraEntity).worldMatrix;

  auto &camera = world.getComponent<CameraComponent>(cameraEntity);

  TransformComponent cameraGlobalT = math::transformFromMatrix(cameraGM);
  for (Entity entity : entities) {
    if (world.hasComponent<GlobalTransform>(entity) &&
        world.hasComponent<MeshComponent>(entity) &&
        world.hasComponent<MaterialComponent>(entity)) {

      Mat4 &globalMat = world.getComponent<GlobalTransform>(entity).worldMatrix;
      auto &meshC = world.getComponent<MeshComponent>(entity);
      if (!meshC.mesh)
        continue;
      auto &material = world.getComponent<MaterialComponent>(entity);

      renderer->renderMesh(meshC.mesh.get(), globalMat, cameraGlobalT, camera,
                           material);
    }
  }
}

void ScriptSystem::update(World &world, float dt) {
  const auto &entities = world.getEntities();
  for (Entity entity : entities) {
    if (world.hasComponent<ScriptComponent>(entity)) {
      auto &sc = world.getComponent<ScriptComponent>(entity);
      for (auto &script : sc.scripts) {
        if (!script->started) {
          script->start();
          script->started = true;
        }
        script->update(dt);
      }
    }
  }
}

void ScriptSystem::start(World &world) {
  const auto &entities = world.getEntities();
  for (Entity entity : entities) {
    if (world.hasComponent<ScriptComponent>(entity)) {
      auto &sc = world.getComponent<ScriptComponent>(entity);
      for (auto &script : sc.scripts) {
        script->start();
        script->started = true;
      }
    }
  }
}

void processEntity(World &world, Entity e, const Mat4 &parentMatrix) {
  auto &local = world.getComponent<TransformComponent>(e);
  Mat4 localMat = Mat4::modelMatrix(local);

  Mat4 globalMat = parentMatrix * localMat;
  world.getComponent<GlobalTransform>(e).worldMatrix = globalMat;

  for (Entity child : world.getChildren(e)) {
    processEntity(world, child, globalMat);
  }
}

void HierarchySystem::update(World &world, float dt) {
  for (Entity e : world.getEntities()) {
    if (!world.hasComponent<ParentComponent>(e)) {
      // This is a root node
      processEntity(world, e, Mat4::identity());
    }
  }
}

void CameraControllerSystem::update(World &world, float dt) {
  Controller* controller = EngineServices::get().getContext()->controller;
  if (!controller)
    return;
  Vec3 moveDir{};
  if (controller->isKeyPressed(Key::W))
    moveDir.z += 1;

  if (controller->isKeyPressed(Key::S))
    moveDir.z -= 1;
  if (controller->isKeyPressed(Key::A))
    moveDir.x -= 1;

  if (controller->isKeyPressed(Key::D))
    moveDir.x += 1;

  if (controller->isKeyPressed(Key::E))
    moveDir.y += 1;

  if (controller->isKeyPressed(Key::Q))
    moveDir.y -= 1;

  moveDir = moveDir.normalized();

  for (Entity e : world.view<CameraControllerComponent, TransformComponent,
                             CameraComponent>()) {

    auto &transform = world.getComponent<TransformComponent>(e);
    auto &cameraC = world.getComponent<CameraControllerComponent>(e);

    Vec3 forward, right, up;
    math::updateCameraBasis(transform.rotation, forward, right, up);

    transform.position =
        transform.position +
        (forward * moveDir.z + right * moveDir.x + up * moveDir.y) *
            cameraC.speed * dt;

    if (controller->rightClick) {

      transform.rotation.y += controller->dx * cameraC.sens * dt;

      transform.rotation.x -= controller->dy * cameraC.sens * dt;
      transform.rotation.x = std::clamp(transform.rotation.x, -1.5f, 1.5f);
    }
  }
  controller->dx = 0;
  controller->dy = 0;
}

// PhysicsSystem

void PhysicsSystem::update(World &world, float dt) {
  auto entities = world.view<RigidBodyComponent, TransformComponent>();
  for (Entity e : entities) {
    auto &rb = world.getComponent<RigidBodyComponent>(e);
    auto &tf = world.getComponent<TransformComponent>(e);

    if (!rb.isKinematic) {
      rb.velocity = rb.velocity + rb.acceleration * dt;
      tf.position = tf.position + rb.velocity * dt;
    }
  }
}

// CollisionSystem

void CollisionSystem::update(World &world, float dt) {
  auto entities = world.view<ColliderComponent, TransformComponent>();

  for (size_t i = 0; i < entities.size(); ++i) {
    for (size_t j = i + 1; j < entities.size(); ++j) {
      Entity a = entities[i];
      Entity b = entities[j];

      auto &colA = world.getComponent<ColliderComponent>(a);
      auto &tfA = world.getComponent<TransformComponent>(a);
      auto &colB = world.getComponent<ColliderComponent>(b);
      auto &tfB = world.getComponent<TransformComponent>(b);

      bool collided = false;

      if (colA.shape == ColliderComponent::Shape::Box &&
          colB.shape == ColliderComponent::Shape::Box) {
        collided = collision::AABBvsAABB(tfA.position, colA.size, tfB.position, colB.size);
      } else if (colA.shape == ColliderComponent::Shape::Sphere &&
                 colB.shape == ColliderComponent::Shape::Sphere) {
        collided = collision::SpherevsSphere(tfA.position, colA.radius, tfB.position,
                                  colB.radius);
      } else { 
        // TODO: implement box-sphere and capsule collisions
        continue;
      }

      if (collided) {
        std::cout << "Collision detected between entities " << a << " and " << b
                  << std::endl;

        // TODO: implement collision response (e.g. separate overlapping
        // entities, reflect velocities, trigger events)
      }
    }
  }
}



// StateSystem

void StateSystem::update(World &world, float dt) {
  auto entities = world.view<StateComponent>();
  for (Entity e : entities) {
    auto &state = world.getComponent<StateComponent>(e);
  }
}

// LifetimeSystem

void LifetimeSystem::update(World &world, float dt) {
  auto entities = world.view<LifetimeComponent>();
  for (Entity e : entities) {
    auto &lifetime = world.getComponent<LifetimeComponent>(e);
    lifetime.timeRemaining -= dt;
    if (lifetime.timeRemaining <= 0.0f) {
      world.destroyEntity(e);
    }
  }
}

// AudioSystem

void AudioSystem::update(World &world, float dt) {
  auto entities = world.view<AudioSourceComponent>();
  for (Entity e : entities) {
    auto &audio = world.getComponent<AudioSourceComponent>(e);
  }
}

// TimerSystem

void TimerSystem::update(World &world, float dt) {
  auto entities = world.view<TimersComponent>();
  for (Entity e : entities) {
    auto &timersComp = world.getComponent<TimersComponent>(e);
    for (auto &pair : timersComp.timers) {
      auto &timer = *pair.second;
      if (!timer.finished) {
        timer.current += dt;
        if (timer.current >= timer.max) {
          timer.finished = true;
          if (timer.repeat) {
            timer.current = 0.0f;
            timer.finished = false;
          }
        }
      }
    }
  }
}

void TimerSystem::addTimer(World &world, Entity e, const std::string &name,
                           float max, bool repeat) {
  if (!world.hasComponent<TimersComponent>(e)) {
    world.addComponent<TimersComponent>(e, TimersComponent{});
  }
  auto &timersComp = world.getComponent<TimersComponent>(e);
  timersComp.timers[name] =
      std::make_shared<Timer>(Timer{0.0f, max, repeat, false, name});
}

void TimerSystem::removeTimer(World &world, Entity e, const std::string &name) {
  if (!world.hasComponent<TimersComponent>(e))
    return;
  auto &timersComp = world.getComponent<TimersComponent>(e);
  timersComp.timers.erase(name);
}

} // namespace farixEngine
