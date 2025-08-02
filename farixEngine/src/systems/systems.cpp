#include "farixEngine/systems/systems.hpp"
#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/components/components.hpp"
#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/core/world.hpp"
#include "farixEngine/ecs/system.hpp"
#include "farixEngine/events/eventDispatcher.hpp"
#include "farixEngine/events/events.hpp"
#include "farixEngine/math/general.hpp"
#include "farixEngine/math/mat4.hpp"
#include "farixEngine/physics/collisionHelpers.hpp"
#include "farixEngine/renderer/helpers.hpp"
#include "farixEngine/renderer/renderer.hpp"
#include "farixEngine/scene/scene.hpp"
#include "farixEngine/scene/sceneManager.hpp"
#include "farixEngine/script/script.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
namespace farixEngine {
using Entity = uint32_t;

Mat4 RenderSystem::getViewMatrix(World &world) {
  Entity cameraEntity = world.getCamera();

  if (!cameraEntity || cameraEntity <= 0)
    return Mat4();

  auto &cameraGlobalMat =
      world.getComponent<GlobalTransform>(cameraEntity).worldMatrix;
  auto cameraTransform = math::transformFromMatrix(cameraGlobalMat);

  Vec3 forward, right, up;
  math::updateCameraBasis(cameraTransform.rotation, forward, right, up);
  return Mat4::lookAt(cameraTransform.position,
                      cameraTransform.position + forward, Vec3(0, 1, 0));
}
Mat4 RenderSystem::getProjectionMatrix(World &world) {
  Entity cameraEntity = world.getCamera();

  if (!cameraEntity || cameraEntity <= 0)
    return Mat4();

  auto &camera = world.getComponent<CameraComponent>(cameraEntity);

  if (camera.mode == CameraProjectionMode::Perspective) {
    return Mat4::perspective(camera.fov, camera.aspectRatio, camera.nearPlane,
                             camera.farPlane);
  } else {
    return Mat4::ortho(camera.orthoLeft, camera.orthoRight, camera.orthoBottom,
                       camera.orthoTop, camera.orthoNear, camera.orthoFar);
  }
}

renderer::RenderContext
RenderSystem::createRenderContext(World &world, const CameraComponent &cam,
                                  const Mat4 &cameraTransform,
                                  const Vec3 &camPosition) {
  renderer::RenderContext ctx;
  ctx.clearColor = 0xFF87CEEB;

  ctx.viewMatrix = getViewMatrix(world);
  ctx.cameraPosition = camPosition;
  ctx.enableLighting = true;
  ctx.enableZBuffer = true;
  ctx.is2DPass = false;

  if (cam.mode == CameraProjectionMode::Perspective) {
    ctx.projectionMatrix = Mat4::perspective(cam.fov, cam.aspectRatio,
                                             cam.nearPlane, cam.farPlane);
    ctx.isOrthographic = false;
  } else {
    ctx.projectionMatrix =
        Mat4::ortho(cam.orthoLeft, cam.orthoRight, cam.orthoBottom,
                    cam.orthoTop, cam.orthoNear, cam.orthoFar);
    ctx.isOrthographic = true;
  }

  return ctx;
}

Vec3 RenderSystem::calculateAnchoredPosition(const RectComponent &rect,
                                             UIComponent::Anchor anchor,
                                             float screenWidth,
                                             float screenHeight) {
  Vec3 pos = rect.position;
  Vec3 offset;

  switch (anchor) {
  case UIComponent::Anchor::TopLeft:
    offset = Vec3(0, 0, 0);
    break;
  case UIComponent::Anchor::TopCenter:
    offset = Vec3(screenWidth / 2, 0, 0);
    break;
  case UIComponent::Anchor::TopRight:
    offset = Vec3(screenWidth, 0, 0);
    break;
  case UIComponent::Anchor::LeftCenter:
    offset = Vec3(0, screenHeight / 2, 0);
    break;
  case UIComponent::Anchor::Center:
    offset = Vec3(screenWidth / 2, screenHeight / 2, 0);
    break;
  case UIComponent::Anchor::RightCenter:
    offset = Vec3(screenWidth, screenHeight / 2, 0);
    break;
  case UIComponent::Anchor::BottomLeft:
    offset = Vec3(0, screenHeight, 0);
    break;
  case UIComponent::Anchor::BottomCenter:
    offset = Vec3(screenWidth / 2, screenHeight, 0);
    break;
  case UIComponent::Anchor::BottomRight:
    offset = Vec3(screenWidth, screenHeight, 0);
    break;
  }

  return offset + pos;
} 
void RenderSystem::onUpdate(World &world, float dt) {
  renderer::Renderer *renderer = EngineServices::get().getContext()->renderer;

  Entity cameraEntity = world.getCamera();
  if (!cameraEntity || cameraEntity <= 0)
    return;

  const auto &camera = world.getComponent<CameraComponent>(cameraEntity);
  const auto &cameraTransform =
      world.getComponent<TransformComponent>(cameraEntity);
  const auto &cameraGlobal =
      world.getComponent<GlobalTransform>(cameraEntity).worldMatrix;
  Vec3 cameraPosition = math::transformFromMatrix(cameraGlobal).position;

  renderer::RenderContext ctx =
      createRenderContext(world, camera, cameraGlobal, cameraPosition);
  renderer->beginFrame(ctx);

  for (Entity entity : world.getEntities()) {
    if (world.hasComponent<GlobalTransform>(entity) &&
        world.hasComponent<MeshComponent>(entity) &&
        world.hasComponent<MaterialComponent>(entity)) {

      const Mat4 &model =
          world.getComponent<GlobalTransform>(entity).worldMatrix;
      auto &meshC = world.getComponent<MeshComponent>(entity);
      auto &matC = world.getComponent<MaterialComponent>(entity);

      if (!meshC.mesh)
        continue;

      renderer::MeshData meshData;
      meshData.positions = meshC.mesh->vertices;
      meshData.normals = meshC.mesh->normals;
      meshData.uvs = meshC.mesh->textureMap;
      for (auto &tri : meshC.mesh->triangles) {
        meshData.indices.push_back(
            renderer::TriangleData{tri.i0, tri.i1, tri.i2, tri.uv0, tri.uv1,
                                   tri.uv2, tri.n0, tri.n1, tri.n2});
      }

      renderer::MaterialData matData;
      matData.baseColor = matC.baseColor;
      matData.ambient = matC.ambient;
      matData.diffuse = matC.diffuse;
      matData.specular = matC.specular;
      matData.shininess = matC.shininess;
      matData.useTexture = matC.useTexture;
      matData.texture = matC.texture.get();
      matData.doubleSided = matC.doubleSided;

      renderer->renderMesh(meshData, model, matData);
    }

    if (world.hasComponent<GlobalTransform>(entity) &&
        world.hasComponent<Sprite2DComponent>(entity)) {
      const Mat4 &model =
          world.getComponent<GlobalTransform>(entity).worldMatrix;
      const auto &sprite = world.getComponent<Sprite2DComponent>(entity);

      renderer::SpriteData spriteData;
      spriteData.texture = sprite.texture.get();
      spriteData.size = sprite.size;
      spriteData.useTexture = sprite.useTexture;
      spriteData.color = sprite.color;
      spriteData.flipX = sprite.flipX;
      spriteData.flipY = sprite.flipY;

      renderer->renderSprite(spriteData, model);
    }
  }
  renderer->beginUIPass(renderer->getScreenSize()[0],
                        renderer->getScreenSize()[1]);

  for (Entity entity : world.view<RectComponent, UIComponent>()) {

    const auto &uiCom = world.getComponent<UIComponent>(entity);
    const auto &uiRect = world.getComponent<RectComponent>(entity);
    const auto &rectPos = calculateAnchoredPosition(
        uiRect, uiCom.anchor, renderer->getScreenSize()[0],
        renderer->getScreenSize()[1]);
    Mat4 model = Mat4::translate(rectPos) * Mat4::rotateZ(uiRect.rotation) *
                 Mat4::scale(Vec3{uiRect.size.x, -uiRect.size.y, 1.0f});

    if (world.hasComponent<UIImageComponent>(entity)) {
      const auto &uiImage = world.getComponent<UIImageComponent>(entity);

      renderer::SpriteData spriteData;
      spriteData.texture = uiImage.texture.get();
      spriteData.size = Vec3(1.f);
      spriteData.useTexture = uiImage.useTexture;
      spriteData.color = uiImage.color;
      spriteData.flipX = false;
      spriteData.flipY = false;

      renderer->renderSprite(spriteData, model);
    }
    if (world.hasComponent<UITextComponent>(entity)) {
      auto &uiText = world.getComponent<UITextComponent>(entity);

      renderer->drawText(uiText.font.get(), uiText.text, uiRect.position,
                         uiText.fontSize, uiText.color);
    }
  }

  renderer->endFrame();
}
void ScriptSystem::onUpdate(World &world, float dt) {
  const auto &entities = world.getEntities();
  for (Entity entity : entities) {
    if (world.hasComponent<ScriptComponent>(entity)) {
      auto &sc = world.getComponent<ScriptComponent>(entity);
      for (auto &script : sc.scripts) {
        if (!script->started) {
          script->onStart();
          script->started = true;
        }
        script->onUpdate(dt);
      }
    }
  }
}

void ScriptSystem::onStart(World &world) {

  const auto &entities = world.getEntities();
  for (Entity entity : entities) {
    if (world.hasComponent<ScriptComponent>(entity)) {
      auto &sc = world.getComponent<ScriptComponent>(entity);
      for (auto &script : sc.scripts) {
        script->onStart();
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

void HierarchySystem::onUpdate(World &world, float dt) {
  for (Entity e : world.getEntities()) {
    if (!world.hasComponent<ParentComponent>(e)) {
      // This is a root node
      processEntity(world, e, Mat4::identity());
    }
  }
}

void CameraControllerSystem::onUpdate(World &world, float dt) {
  Controller *controller = EngineServices::get().getContext()->controller;
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

void PhysicsSystem::onUpdate(World &world, float dt) {
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

void CollisionSystem::onUpdate(World &world, float dt) {
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
        collided = collision::AABBvsAABB(tfA.position, colA.size, tfB.position,
                                         colB.size);
      } else if (colA.shape == ColliderComponent::Shape::Sphere &&
                 colB.shape == ColliderComponent::Shape::Sphere) {
        collided = collision::SpherevsSphere(tfA.position, colA.radius,
                                             tfB.position, colB.radius);
      } else {
        // TODO: implement box-sphere and capsule collisions
        continue;
      }

      if (collided) {
        // std::cout << "Collision detected between entities " << a << " and "
        // << b
        //           << std::endl;

        GameWorld &gworld = EngineServices::get()
                                .getContext()
                                ->sceneManager->currentScene()
                                ->gameWorld();
        CollisionEvent collision(&gworld.getGameObject(a),
                                 &gworld.getGameObject(b));
        EngineServices::get().getEventDispatcher().emit(collision);

        // TODO: implement collision response (e.g. separate overlapping
        // entities, reflect velocities, trigger events)
      }
    }
  }
}

// StateSystem

void StateSystem::onUpdate(World &world, float dt) {
  auto entities = world.view<StateComponent>();
  for (Entity e : entities) {
    auto &state = world.getComponent<StateComponent>(e);
  }
}

// LifetimeSystem

void LifetimeSystem::onUpdate(World &world, float dt) {
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

void AudioSystem::onUpdate(World &world, float dt) {
  auto entities = world.view<AudioSourceComponent>();
  for (Entity e : entities) {
    auto &audio = world.getComponent<AudioSourceComponent>(e);
  }
}

// TimerSystem

void TimerSystem::onUpdate(World &world, float dt) {
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

void BillboardSystem::onUpdate(World &world, float dt) {
  auto cameraEntities = world.view<CameraComponent, TransformComponent>();
  if (cameraEntities.empty())
    return;

  Entity camEntity = cameraEntities.front();
  // auto &camTransform = world.getComponent<TransformComponent>(camEntity);
  auto &camGlobal = world.getComponent<GlobalTransform>(camEntity).worldMatrix;
  Vec3 cameraPos = math::transformFromMatrix(camGlobal).position;

  for (Entity e : world.view<TransformComponent, BillboardComponent>()) {
    auto &tf = world.getComponent<TransformComponent>(e);
    auto &bb = world.getComponent<BillboardComponent>(e);

    Vec3 toCamera = (cameraPos - tf.position).normalized();

    switch (bb.type) {
    case BillboardComponent::BillboardType::BillboardY: {

      float yaw = std::atan2(toCamera.x, toCamera.z);
      tf.rotation = Vec3(0, yaw, 0);
      break;
    }

    case BillboardComponent::BillboardType::BillboardFull: {

      Vec3 forward = toCamera;
      Vec3 up(0, 1, 0);

      float pitch = -std::atan2(
          forward.y, std::sqrt(forward.x * forward.x + forward.z * forward.z));
      float yaw = std::atan2(forward.x, forward.z);

      tf.rotation = Vec3(pitch, yaw, 0);
      break;
    }

    default:
      break;
    }
  }
}

} // namespace farixEngine
