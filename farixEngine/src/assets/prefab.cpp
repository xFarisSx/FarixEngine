#include "farixEngine/assets/prefab.hpp"
#include "farixEngine/API/gameObject.hpp"
#include "farixEngine/API/gameWorld.hpp"

#include "farixEngine/core/engineServices.hpp"
#include "farixEngine/script/script.hpp"
#include "farixEngine/thirdparty/nlohmann/json.hpp"
#include <fstream>
#include <iostream>
using json = nlohmann::json;

namespace farixEngine {

void Prefab::save(GameObject &obj, const std::string &path) {
  Serializer::savePrefab(obj, path);
}

GameObject &Prefab::instantiate(GameWorld &gworld, const std::string &path) {
  GameObject &root = Serializer::loadPrefab(gworld, path);

  return root;
}
} // namespace farixEngine
