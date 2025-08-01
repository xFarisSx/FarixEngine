#pragma once

#include <memory>
#include <string>

#include <vector>

namespace farixEngine {

class World;
class System {
public:
  bool started = false;
  std::string name = "System";

  System(const std::string &systemName) : name(systemName) {}
  virtual void start(World &world) = 0;
  virtual void update(World &world, float dt) = 0;
  virtual ~System() = default;
};

class SystemManager {
public:
  void addSystem(std::shared_ptr<System> system);
  void updateAll(World &world, float dt);
  void startAll(World &world);
  std::vector<std::shared_ptr<System>> getAll();
  void clearSystems();

private:
  std::vector<std::shared_ptr<System>> systems;
}; 

} // namespace farixEngine
