

#include "farixEngine/API/gameWorld.hpp"
#include "farixEngine/core/engineContext.hpp"

namespace farixEngine {

GameWorld::GameWorld(World *world) : _world(world) {}

GameWorld::~GameWorld() {}

GameObject &GameWorld::createGameObject() {
  Entity e = _world->createEntity();
  auto &obj = gameObjects[e] = GameObject(this, e);
  return obj;
}

GameObject &GameWorld::createSprite3D(std::shared_ptr<Texture> texture,
                                      Vec3 size) {
  GameObject &obj = createGameObject();

  obj.setMesh(Mesh::createQuad(size));

  MaterialComponent mat;
  mat.useTexture = true;
  mat.texture = texture;
  obj.setMaterial(mat);

  return obj;
}

GameObject &GameWorld::createSprite2D(std::shared_ptr<Texture> texture,
                                      Vec3 size) {
  GameObject &obj = createGameObject();
  Sprite2DComponent sprite;

  sprite.useTexture = true;
  sprite.texture = texture;
  sprite.size = size;

  obj.addComponent<Sprite2DComponent>(sprite);

  return obj;
}

GameObject &GameWorld::registerExistingEntity(Entity e) {
  if (gameObjects.count(e))
    return gameObjects.at(e);
  auto &obj = gameObjects[e] = GameObject(this, e);
  return obj;
}

GameObject &GameWorld::getGameObject(Entity entity) {
  return gameObjects.at(entity);
}

std::vector<GameObject *> GameWorld::getAllGameObjects() {
  std::vector<GameObject *> list;
  for (auto &[e, obj] : gameObjects)
    list.push_back(&obj);
  return list;
}

std::vector<GameObject *>
GameWorld::getGameObjectsByName(const std::string &name) {
  std::vector<GameObject *> list;
  for (auto &[e, obj] : gameObjects)
    if (obj.getName() == name)
      list.push_back(&obj);
  return list;
}
void GameWorld::destroyObject(GameObject &go) {
  gameObjects.erase(go.getEntity());
  _world->destroyEntity(go.getEntity());
}
void GameWorld::setCamera(const GameObject &camera) {
  _world->setCameraEntity(camera.getEntity());
}

GameObject &GameWorld::getCamera() {
  Entity camEntity = _world->getCamera();

  return getGameObject(camEntity);
}

void GameWorld::clear() {
  std::vector<Entity> toDestroy;
  for (const auto &[e, obj] : gameObjects)
    toDestroy.push_back(e);

  for (Entity e : toDestroy)
    gameObjects[e].destroyObject();
}

World *GameWorld::getInternalWorld() { return _world; }

const World *GameWorld::getInternalWorld() const { return _world; }

void GameWorld::addSystem(std::shared_ptr<System> sys) {
  _world->addSystem(sys);
}

} // namespace farixEngine
