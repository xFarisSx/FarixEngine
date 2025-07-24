#include "farixEngine/ecs/system.hpp"
#include "farixEngine/core/world.hpp"
namespace farixEngine {

    void SystemManager::addSystem(std::shared_ptr<System> system) {
        systems.push_back(system);
    }

    void SystemManager::updateAll(World& world, float dt) {
        for (auto& system : systems) {
            system->update(world, dt);

        }
    }

    void SystemManager::startAll(World& world) {
        for (auto& system : systems) {
            system->start(world);
        }
    }

std::vector<std::shared_ptr<System>> SystemManager::getAll(){return systems;}


}  
