
#include "game.hpp"
#include <memory>

void setupScene(GameWorld &gameWorld) {
  // Ball
  // auto& ball = gameWorld.createGameObject();
  // ball.setName("Ball");
  // ball.setMesh(Mesh::createSphere(0.2f, 8, 16));
  // ball.setMaterial(MaterialComponent{}); 
  // ball.getComponent<TransformComponent>().position = Vec3(0, 0, 0);
  // ball.addScript(std::make_shared<BallScript>());
  // ball.addComponent<BlinkComponent>({0.0f, 0.5f, true});
  auto& ball = Prefab::instantiate(gameWorld, "prefabs/ball.json");

  // Player Paddle
  // auto& paddle1 = gameWorld.createGameObject();
  // paddle1.setName("Player");
  // paddle1.setMesh(Mesh::createBox(2.0f, 0.2f, 0.5f));
  // paddle1.setMaterial(MaterialComponent{});
  // paddle1.getComponent<TransformComponent>().position = Vec3(0, 5, 0);
  // paddle1.addTag("Paddle");
  // paddle1.addScript(std::make_shared<PlayerPaddleScript>());
  auto& paddle1 = Prefab::instantiate(gameWorld, "prefabs/paddle.json");

  // Opponent Paddle
  // auto& paddle2 = gameWorld.createGameObject();
  // paddle2.setName("Opponent");
  // paddle2.setMesh(Mesh::createBox(2.0f, 0.2f, 0.5f));
  // paddle2.setMaterial(MaterialComponent{});
  // paddle2.getComponent<TransformComponent>().position = Vec3(0, -5, 0);
  // paddle2.addTag("Paddle");
  // paddle2.addScript(std::make_shared<OpponentPaddleScript>());
  auto& paddle2 = Prefab::instantiate(gameWorld, "prefabs/paddle.json");
  paddle2.setName("Opponent");
  paddle2.removeScriptByName("PlayerPaddleScript");
  paddle2.addScript(std::make_shared<OpponentPaddleScript>());
  paddle2.getComponent<TransformComponent>().position = Vec3(0, -5, 0);

  // Camera
  auto& camera = gameWorld.createGameObject();
  camera.getComponent<TransformComponent>().position = Vec3(0, 0, -7);
  camera.addComponent<CameraComponent>();

  gameWorld.setCamera(camera);

   //
  // Prefab::save(paddle1, "prefabs/paddle.json");
  // Prefab::save(ball, "prefabs/ball.json");
}

void Game::onStart() {
  auto &engine = EngineServices::get().getEngineRegistry();
  engine.getScriptRegistry().registerScript<BallScript>("BallScript");
  engine.getScriptRegistry().registerScript<PlayerPaddleScript>(
      "PlayerPaddleScript");
  engine.getScriptRegistry().registerScript<OpponentPaddleScript>(
      "OpponentPaddleScript");
  engine.getSystemRegistry().registerSystem<BlinkSystem>("BlinkSystem");

  engine.getSerializerRegistry().registerSerializer<BlinkComponent>(
      "BlinkComponent",

      [](World &world, Entity e) -> json {
        const auto &blink = world.getComponent<BlinkComponent>(e);
        return json{{"timer", blink.timer},
                    {"interval", blink.interval},
                    {"visible", blink.visible}};
      },
      [](World &world, Entity e, const json &j) {
        BlinkComponent blink;
        blink.timer = j.value("timer", 0.0f);
        blink.interval = j.value("interval", 0.5f);
        blink.visible = j.value("visible", true);
        world.addComponent(e, blink);
      });

  SceneManager &sceneManager = getSceneManager();

  auto &scene = sceneManager.createScene("pong");
  scene.gameWorld().registerComponent<BlinkComponent>();

  setupScene(*sceneManager.currentGameWorld());

  sceneManager.currentGameWorld()->addSystem(std::make_shared<BlinkSystem>());

  sceneManager.saveCurrentScene("scenes/");
}

void Game::onUpdate(float dt) {}
