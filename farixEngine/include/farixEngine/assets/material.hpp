#pragma once
#include "farixEngine/assets/assetManager.hpp"
#include "farixEngine/math/vec4.hpp"
#include "farixEngine/utils/uuid.hpp"

#include <string>

#include <memory>

namespace farixEngine {

struct Material : public Asset {
  std::string id;
  std::string path;

  Vec4 baseColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
  float ambient = 0.1f;
  float specular = 0.5f;
  float shininess = 32.0f;
  float diffuse = 1.0f;
  AssetID texture = "";

  bool useTexture = false;
  bool doubleSided = true;

  Material(UUID uuid = "") : id(uuid.empty() ? utils::generateUUID() : uuid) {} 
};
} // namespace farixEngine
