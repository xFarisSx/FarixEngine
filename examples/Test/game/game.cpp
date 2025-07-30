#include "game.hpp"

void setupScene(GameWorld &gameWorld) {
  // Ground (static)
  // auto &ground = gameWorld.createGameObject();
  auto &ground = gameWorld.createSprite3D(
      Texture::loadFromBmp("assets/textures/textcat.bmp"), Vec3(10, 10, 0));
  ground.setName("Ground");
  // ground.setMesh(Mesh::createBox(10.0f, 0.5f, 1.0f));
  // ground.setMaterial(MaterialComponent{});
  ground.getComponent<TransformComponent>().position = Vec3(0, -3, 0);
  ground.addComponent<ColliderComponent>(
      {ColliderComponent::Shape::Box, Vec3(10.0f, 0.5f, 1.0f)});
  ground.getComponent<MaterialComponent>().doubleSided = false;

  ground.emplaceComponent<BillboardComponent>(
      BillboardComponent::BillboardType::BillboardFull);

  // Falling Box (dynamic)
  auto &box = gameWorld.createGameObject();
  box.setName("Box");
  box.setMesh(Mesh::createBox(1.0f, 1.0f, 1.0f));
  box.setMaterial(MaterialComponent{});
  box.getComponent<TransformComponent>().position = Vec3(0, 3, 0);

  box.addComponent<RigidBodyComponent>(
      {.velocity = Vec3(0, 0, 0),
       .acceleration = Vec3(0, -9.81f, 0), // Gravity
       .mass = 1.0f,
       .isKinematic = false});
  box.addComponent<ColliderComponent>(
      {ColliderComponent::Shape::Box, Vec3(1.0f, 1.0f, 1.0f)});
  box.addComponent<LifetimeComponent>({5.0f}); // Destroy after 5 seconds
  box.addScript(std::make_shared<TestCollisionScript>());

  // Camera
  auto &camera = gameWorld.createGameObject();
  camera.setName("Camera");
  camera.getComponent<TransformComponent>().position = Vec3(0, 0, 10);
  camera.addComponent<CameraComponent>().getComponent<CameraComponent>().mode =
      CameraProjectionMode::Orthographic;
  camera.getComponent<CameraComponent>().setOrthoZoom(20);
  camera.addComponent<CameraControllerComponent>();
  camera.setParent(box);

  gameWorld.setCamera(camera);
}
void Game::onStart() {
  auto &engine = EngineServices::get().getEngineRegistry();
  engine.getScriptRegistry().registerScript<TestCollisionScript>(
      "TestCollisionScript");

  // Create scene
  SceneManager &sceneManager = getSceneManager();
  auto &scene = sceneManager.createScene("physics_test");
  // auto &scene = sceneManager.loadSceneFromFile("scenes/physics_test.json");
  GameWorld &world = *sceneManager.currentGameWorld();

  setupScene(world);

  sceneManager.saveCurrentScene("scenes/");
}

void Game::onUpdate(float dt) {}
