
#include "game.hpp"

void setupScene(GameWorld &gameWorld) {
  // Create a box object
  auto box = gameWorld.createGameObject();
  box.getComponent<TransformComponent>().position = Vec3(4, 2, 1);
  box.setMesh(Mesh::createBox(1.0, 2.0, 1.0));
  box.setName("Box");
  box.addTags("Obstacle");

  MaterialComponent boxMat;
  boxMat.useTexture = false;
  box.setMaterial(boxMat);

  // Create a camera object
  auto camera = gameWorld.createGameObject();
  camera.getComponent<TransformComponent>().position = Vec3(0, 0, -5);
  camera.addComponents<CameraComponent, CameraControllerComponent>(
      CameraComponent{float(M_PI) / 2, 16.f / 9.f, 1.0f, 30.f},
      CameraControllerComponent{});
  gameWorld.setCamera(camera);

  // Create a model object
  auto model = gameWorld.createGameObject();
  model.getComponent<TransformComponent>().scale = Vec3(0.01f);
  model.setMesh(Mesh::loadFromObj("assets/models/cat.obj"));

  MaterialComponent modelMat;
  modelMat.texture = Texture::loadFromBmp("assets/textures/textcat1.bmp");
  modelMat.useTexture = true;
  model.setMaterial(modelMat);

  model.addScript(std::make_shared<Rotator>());

  // Create a sphere object
  auto sphere = gameWorld.createGameObject();
  sphere.setMesh(Mesh::createSphere(1, 16, 32));
  sphere.setMaterial(MaterialComponent{});
}

void Game::onStart() {
  // Register scripts here so GameWorld can instantiate by name
  EngineRegistry::get().getScriptRegistry().registerScript<Rotator>("Rotator");

  Scene &mainScene = sceneManager.createScene("main");
  // Setup the scene programmatically
  setupScene(*sceneManager.currentGameWorld());

  // sceneManager.saveCurrentScene("scenes/test.json");

  // sceneManager.loadSceneFromFile("scenes/test.json");
  // sceneManager.saveCurrentScene("scenes/test.json");
}

void Game::onUpdate(float dt) {}
