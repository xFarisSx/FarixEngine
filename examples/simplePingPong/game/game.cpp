
#include "game.hpp"
#include "components/components.hpp"
#include <memory>

void setupScene3D(GameWorld &gameWorld) {
  // Ball
  auto &ball = gameWorld.createGameObject();
  ball.setName("Ball");
  ball.setMesh(Mesh::createSphere(0.2f, 8, 16));
  ball.setMaterial(MaterialComponent{});
  ball.getComponent<TransformComponent>().position = Vec3(0, 0, 0);
  ball.addScript(std::make_shared<BallScript>());
  ball.addComponent<BlinkComponent>({0.0f, 0.5f, true});
  // auto &ball = Prefab::instantiate(gameWorld, "prefabs/ball.json");

  // Player Paddle
  auto &paddle1 = gameWorld.createGameObject();
  paddle1.setName("Player");
  paddle1.setMesh(Mesh::createBox(2.0f, 0.2f, 0.5f));
  paddle1.setMaterial(MaterialComponent{});
  paddle1.getComponent<TransformComponent>().position = Vec3(0, 5, 0);
  paddle1.addTag("Paddle");
  paddle1.addScript(std::make_shared<PlayerPaddleScript>());
  // auto &paddle1 = Prefab::instantiate(gameWorld, "prefabs/paddle.json");

  // Opponent Paddle
  auto &paddle2 = gameWorld.createGameObject();
  paddle2.setName("Opponent");
  paddle2.setMesh(Mesh::createBox(2.0f, 0.2f, 0.5f));
  paddle2.setMaterial(MaterialComponent{});
  paddle2.getComponent<TransformComponent>().position = Vec3(0, -5, 0);
  paddle2.addTag("Paddle");
  paddle2.addScript(std::make_shared<OpponentPaddleScript>());
  // auto &paddle2 = Prefab::instantiate(gameWorld, "prefabs/paddle.json");
  // paddle2.setName("Opponent");
  // paddle2.removeScriptByName("PlayerPaddleScript");
  // paddle2.addScript(std::make_shared<OpponentPaddleScript>());
  // paddle2.getComponent<TransformComponent>().position = Vec3(0, -5, 0);

  // Camera
  auto &camera = gameWorld.createGameObject();
  camera.getComponent<TransformComponent>().position = Vec3(0, 0, 7);
  camera.addComponent<CameraComponent>().getComponent<CameraComponent>().mode =
      CameraProjectionMode::Perspective;
  camera.getComponent<CameraComponent>().setOrthoZoom(7);

  gameWorld.setCamera(camera);

  //
  // Prefab::save(paddle1, "prefabs/paddle.json");
  // Prefab::save(ball, "prefabs/ball.json");
}

void setupScene2D(GameWorld &gameWorld) {
  // Ball
  // auto &ball = Prefab::instantiate(gameWorld, "prefabs/ball.json");
  auto &ball = gameWorld.createSprite2D(
      Texture::loadFromBmp("assets/textures/textcat.bmp"), Vec3(0.4f, 0.4f, 0));
  ball.setName("Ball");
  ball.getComponent<TransformComponent>().position = Vec3(0, 0, 0);
  ball.addScript(std::make_shared<BallScript>());
  ball.addComponent<BlinkComponent>({0.0f, 0.5f, true});

  // Player Paddle
  // auto &paddle1 = Prefab::instantiate(gameWorld, "prefabs/paddle.json");
  auto &paddle1 = gameWorld.createSprite2D(
      Texture::loadFromBmp("assets/textures/textcat.bmp"), Vec3(2.0f, 0.2f, 0));
  paddle1.setName("Player");
  paddle1.getComponent<TransformComponent>().position = Vec3(0, 5, 0);
  paddle1.addTag("Paddle");
  paddle1.addScript(std::make_shared<PlayerPaddleScript>());

  // Opponent Paddle
  // auto &paddle2 = Prefab::instantiate(gameWorld, "prefabs/paddle.json");
  // paddle2.setName("Opponent");
  // paddle2.removeScriptByName("PlayerPaddleScript");
  // paddle2.addScript(std::make_shared<OpponentPaddleScript>());
  // paddle2.getComponent<TransformComponent>().position = Vec3(0, -5, 0);
  auto &paddle2 = gameWorld.createSprite2D(
      Texture::loadFromBmp("assets/textures/textcat.bmp"), Vec3(2.0f, 0.2f, 1));
  paddle2.setName("Opponent");
  paddle2.getComponent<TransformComponent>().position = Vec3(0, -5, 0);
  paddle2.addTag("Paddle");
  paddle2.addScript(std::make_shared<OpponentPaddleScript>());

  // Camera
  auto &camera = gameWorld.createGameObject();
  camera.getComponent<TransformComponent>().position = Vec3(0, 0, 7);
  camera.addComponent<CameraComponent>().getComponent<CameraComponent>().mode =
      CameraProjectionMode::Orthographic;
  camera.getComponent<CameraComponent>().setOrthoZoom(7);

  gameWorld.setCamera(camera);

  //
  // Prefab::save(paddle1, "prefabs/paddle.json");
  // Prefab::save(ball, "prefabs/ball.json");
}

void setupUI(GameWorld &gameWorld, std::shared_ptr<Font> font) {
  auto &scoreUI = gameWorld.createGameObject();
  scoreUI.addComponent<ScoreComponent>();
  scoreUI.setName("ScoreUI");
  scoreUI.addComponent<UITextComponent>({.text = "Player: 0  Opponent: 0",
                                         .color = Vec4(1, 1, 1, 1),
                                         .fontSize = 24.0f,
                                         .font = font});

  scoreUI.addComponent<RectComponent>({Vec3(10, 10, 0), Vec3(100, 100, 0), 0});
  scoreUI.addComponent<UIComponent>();
}

void Game::onStart() {
  auto &engine = EngineServices::get().getEngineRegistry();
  engine.getScriptRegistry().registerScript<BallScript>("BallScript");
  engine.getScriptRegistry().registerScript<PlayerPaddleScript>(
      "PlayerPaddleScript");
  engine.getScriptRegistry().registerScript<OpponentPaddleScript>(
      "OpponentPaddleScript");
  engine.getSystemRegistry().registerSystem<BlinkSystem>("BlinkSystem");
  engine.getSystemRegistry().registerSystem<ScoreSystem>("ScoreSystem");

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
  scene.gameWorld().registerComponent<ScoreComponent>();

  //
  setupScene3D(*sceneManager.currentGameWorld());
  // setupScene2D(*sceneManager.currentGameWorld());
  auto font = Font::loadFont("assets/fonts/font.ttf", 32);
  setupUI(*sceneManager.currentGameWorld(), font);

  sceneManager.currentGameWorld()->addSystem(std::make_shared<BlinkSystem>());
  sceneManager.currentGameWorld()->addSystem(std::make_shared<ScoreSystem>());

  sceneManager.saveCurrentScene("scenes/");
}

void Game::onUpdate(float dt) {}
