
#pragma once
#include <farixEngine/farixEngine.hpp>

using namespace farixEngine;

struct ScoreEvent : Event {
  bool byPlayer = false;
  ScoreEvent(bool bp) : byPlayer(bp) {}
};

struct ScoreComponent {
  int playerScore = 0;
  int opponentScore = 0;
};

class BallScript : public Script {
  TransformComponent *transform = nullptr;
  EngineContext *ctx = nullptr;
  Vec3 velocity = Vec3(3.0f, 3.0f, 0);

public:
  BallScript() : Script("BallScript") {}

  void onStart() override {

    transform = &getGameObject()->getComponent<TransformComponent>();
    ctx = EngineServices::get().getContext();
  }

  void onUpdate(float dt) override {
    if (!transform)
      return;

    transform->position = transform->position + velocity * dt;

    // Wall bounce
    if (transform->position.x >= 5 || transform->position.x <= -5)
      velocity.x *= -1;

    // Fake paddle bounce
    auto *world = getGameWorld();
    for (auto &paddle : world->getGameObjectsByTags("Paddle")) {
      auto &paddleTransform = paddle->getComponent<TransformComponent>();

      float px = paddleTransform.position.x;
      float py = paddleTransform.position.y;

      Vec3 ballPos = transform->position;
      if (std::abs(ballPos.x - px) < 1.0f && std::abs(ballPos.y - py) < 0.5f) {
        velocity.y *= -1;
        break;
      }
    }

    // Check if ball passed player
    if (transform->position.y > 6.0f) {
      // std::cout << "Game Over, Down won\n";
      // velocity = Vec3(0); // stop
      // ctx->sceneManager->reloadScene();
      transform->position = Vec3(0);
      ScoreEvent e(false);
      EngineServices::get().getEventDispatcher().emit(e);

    } else if (transform->position.y < -6.0f) {
      // std::cout << "Game Over, Up won\n";
      // velocity = Vec3(0); // stop 
      // ctx->sceneManager->reloadScene();
      transform->position = Vec3(0);
      ScoreEvent e(true);
      EngineServices::get().getEventDispatcher().emit(e);
    }
  }
};

class PlayerPaddleScript : public Script {
  TransformComponent *transform = nullptr;
  EngineContext *ctx = nullptr;

public:
  PlayerPaddleScript() : Script("PlayerPaddleScript") {}

  void onStart() override {
    transform = &getGameObject()->getComponent<TransformComponent>();
    ctx = EngineServices::get().getContext();
  }

  void onUpdate(float dt) override {
    float speed = 5.0f;
    if (ctx->controller->isKeyPressed(Key::A))
      transform->position.x -= speed * dt;
    if (ctx->controller->isKeyPressed(Key::D))
      transform->position.x += speed * dt;

    // Clamp position
    transform->position.x = std::clamp(transform->position.x, -4.5f, 4.5f);
  }
};

class OpponentPaddleScript : public Script {
  TransformComponent *transform = nullptr;
  EngineContext *ctx = nullptr;

public:
  OpponentPaddleScript() : Script("OpponentPaddleScript") {}

  void onStart() override {
    transform = &getGameObject()->getComponent<TransformComponent>();
    ctx = EngineServices::get().getContext();
  }

  void onUpdate(float dt) override {
    float speed = 5.0f;
    if (ctx->controller->isKeyPressed(Key::Left))
      transform->position.x -= speed * dt;
    if (ctx->controller->isKeyPressed(Key::Right))
      transform->position.x += speed * dt;

    // Clamp position so it stays in bounds
    transform->position.x = std::clamp(transform->position.x, -4.5f, 4.5f);
  }
};

//// Example user defined component and system
struct BlinkComponent {
  float timer = 0.0f;
  float interval = 0.2f; // seconds between blinks
  bool visible = true;
};
class BlinkSystem : public System {
public:
  BlinkSystem() : System("BlinkSystem") {}

  void onStart(World &world) override {}

  void onUpdate(World &world, float dt) override {
    for (auto entity : world.view<BlinkComponent>()) {
      auto &blink = world.getComponent<BlinkComponent>(entity);
      blink.timer += dt;
      if (blink.timer >= blink.interval) {
        blink.visible = !blink.visible;
        blink.timer = 0.0f;

        if (world.hasComponent<MaterialComponent>(entity)) {
          auto &mat = world.getComponent<MaterialComponent>(entity);
          if (blink.visible) {
            mat.overrides.baseColor = Vec4(1, 1, 1, 1);
          } else {
            mat.overrides.baseColor = Vec4(1, 0, 0, 0.6f);
          }
        }
      }
    }
  }
};

class ScoreSystem : public System {
public:
  int playerScore = 0;
  int opponentScore = 0;

  ScoreSystem() : System("ScoreSystem") {}

  void onStart(World &world) override {
    EngineServices::get().getEventDispatcher().listen<ScoreEvent>(
        [this](ScoreEvent &e) { this->onScored(e); });
  }

  void onScored(ScoreEvent &event) {

    if (event.byPlayer)
      playerScore++;
    else
      opponentScore++;
  }

  void onUpdate(World &world, float dt) override {

    // Update UITextComponent(s) with score text
    for (auto &uiEntity : world.view<UITextComponent>()) {
      auto &uiText = world.getComponent<UITextComponent>(uiEntity);
      uiText.text = "Player: " + std::to_string(playerScore) +
                    "  Opponent: " + std::to_string(opponentScore);
    }
  }
};

class Game : public Application {
public:
  void onStart() override;
  void onUpdate(float dt) override;
};
