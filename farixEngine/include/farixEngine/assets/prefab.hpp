
#pragma once

#include <string>


namespace farixEngine {
class GameObject;
class GameWorld;
class Prefab {
public:
    static void save(GameObject& obj, const std::string& path);
    static GameObject instantiate(GameWorld& world, const std::string& path);
};
} // namespace far
