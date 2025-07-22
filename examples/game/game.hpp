
#pragma once
#include "farixEngine/farixEngine.hpp"

using namespace farixEngine;

class Rotator : public Script {
  TransformComponent* transform = nullptr;
  EngineContext* ctx = nullptr;

public:
  Rotator() : Script("Rotator") {}

  void start() override {
    auto obj = getGameObject();
    transform = &obj.getComponent<TransformComponent>();
    ctx = world->getContext();
  }

  void update(float dt) override {
    if (ctx && ctx->controller->isKeyPressed(Key::Escape)) {
      transform->rotation.y += dt;
    }
  }
};

class Game : public Application {
public:
  void onStart() override;
  void onUpdate(float dt) override;
};

