#pragma once
#include <string>
#include "farixEngine/scene/scene.hpp"
#include "farixEngine/API/gameWorld.hpp"

namespace farixEngine {


class SceneManager {
public:

  Scene& createScene(const std::string& name, bool activate=true);
  Scene& loadSceneFromFile(const std::string& filePath, bool activate=true);
  void saveCurrentScene(const std::string& filePath);
  void saveScene(const std::string& name,const std::string& filePath);


  Scene& activateScene(const std::string& name);
  void unloadScene(const std::string& name);
  Scene& getScene(const std::string& name);

  Scene* currentScene();
  GameWorld* currentGameWorld();
  World* currentWorld();

  void setContext(EngineContext* _ctx);

private:
  std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
  Scene* activeScene = nullptr;
  EngineContext* ctx;
};

}
