#pragma once
#include "farixEngine/math/mat4.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/math/vec4.hpp"
#include "farixEngine/scene/scene.hpp"

#include "farixEngine/assets/font.hpp"
#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/assets/texture.hpp"

#include "farixEngine/thirdparty/nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;

namespace farixEngine {

inline void to_json(nlohmann::json &j, const Vec3 &v) {
  j = nlohmann::json{{"x", v.x}, {"y", v.y}, {"z", v.z}};
}
inline void from_json(const nlohmann::json &j, Vec3 &v) {
  j.at("x").get_to(v.x);
  j.at("y").get_to(v.y);
  j.at("z").get_to(v.z);
}

inline void to_json(nlohmann::json &j, const Vec4 &v) {
  j = nlohmann::json{{"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w}};
}
inline void from_json(const nlohmann::json &j, Vec4 &v) {
  j.at("x").get_to(v.x);
  j.at("y").get_to(v.y);
  j.at("z").get_to(v.z);
  j.at("w").get_to(v.w);
}

inline void to_json(nlohmann::json &j, const Mat4 &m) {
  j = nlohmann::json::array();
  for (int row = 0; row < 4; ++row) {
    nlohmann::json rowArr = nlohmann::json::array();
    for (int col = 0; col < 4; ++col) {
      rowArr.push_back(m[row][col]);
    }
    j.push_back(rowArr);
  }
}
inline void from_json(const nlohmann::json &j, Mat4 &m) {
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      m[row][col] = j.at(row).at(col).get<float>();
    }
  }
}

inline void to_json(nlohmann::json &j, const MaterialOverrides &v) {
  j = nlohmann::json::object();
  if (v.baseColor)
    j["baseColor"] = *v.baseColor;
  if (v.ambient)
    j["ambient"] = *v.ambient;
  if (v.specular)
    j["specular"] = *v.specular;
  if (v.shininess)
    j["shininess"] = *v.shininess;
  if (v.diffuse)
    j["diffuse"] = *v.diffuse;
  if (v.texture)
    j["texture"] = *v.texture;
  if (v.useTexture)
    j["useTexture"] = *v.useTexture;
  if (v.doubleSided)
    j["doubleSided"] = *v.doubleSided;
}

inline void from_json(const nlohmann::json &j, MaterialOverrides &v) {
  if (j.contains("baseColor"))
    v.baseColor = j.at("baseColor").get<Vec4>();
  if (j.contains("ambient"))
    v.ambient = j.at("ambient").get<float>();
  if (j.contains("specular"))
    v.specular = j.at("specular").get<float>();
  if (j.contains("shininess"))
    v.shininess = j.at("shininess").get<float>();
  if (j.contains("doubleSided"))
    v.doubleSided = j.at("doubleSided").get<bool>();
  if (j.contains("diffuse"))
    v.diffuse = j.at("diffuse").get<float>();
  if (j.contains("texture"))
    v.texture = j.at("texture").get<AssetID>();
  if (j.contains("useTexture"))
    v.useTexture = j.at("useTexture").get<bool>();
}

class World;

class Serializer {
public:
  static void saveScene(Scene *scene, const std::string &filepath);
  static void loadScene(Scene *scene, const std::string &filepath);
  static void savePrefab(GameObject &obj, const std::string &filepath);
  static GameObject &loadPrefab(GameWorld &gworld, const std::string &filepath);
  template <typename T>
  static json serializeAsset(std::shared_ptr<T> asset,
                             AssetManager &assetManager);
  template <typename T>
  static std::shared_ptr<T> deserializeAsset(const json &j, AssetManager &am);
};
template <typename T>
json Serializer::serializeAsset(std::shared_ptr<T> asset,
                                AssetManager &assetManager) {
  json j;

  if constexpr (std::is_same_v<T, Mesh>) {
    j["uuid"] = asset->id;
    j["name"] = assetManager.findNameById(asset->id).value_or("");
    j["path"] = asset->path;
    j["type"] = asset->type;
    j["size"] = asset->size;
    j["sphereData"] = asset->sphereData;
  } else if constexpr (std::is_same_v<T, Texture>) {
    j["uuid"] = asset->id;
    j["name"] = assetManager.findNameById(asset->id).value_or("");
    j["path"] = asset->path;
  } else if constexpr (std::is_same_v<T, Material>) {
    j["uuid"] = asset->id;
    j["name"] = assetManager.findNameById(asset->id).value_or("");

    j["baseColor"] = asset->baseColor;
    j["ambient"] = asset->ambient;
    j["specular"] = asset->specular;
    j["shininess"] = asset->shininess;
    j["diffuse"] = asset->diffuse;
    j["texture"] = asset->texture;

    j["useTexture"] = asset->useTexture;
    j["doubleSided"] = asset->doubleSided;

  } else if constexpr (std::is_same_v<T, Font>) {
    j["uuid"] = asset->id;
    j["name"] = assetManager.findNameById(asset->id).value_or("");
    j["path"] = asset->path;
    j["ptsize"] = asset->ptsize;
  } else {
    static_assert(false, "Unsupported asset type");
  }

  return j;
}
template <typename T>
std::shared_ptr<T> Serializer::deserializeAsset(const json &j,
                                                AssetManager &am) {
  std::shared_ptr<T> asset;

  std::string uuid = j.value("uuid", "");
  std::string name = j.value("name", "");
  if (am.has(uuid))
    return nullptr;

  if constexpr (std::is_same_v<T, Mesh>) {

    std::string path = j.value("path", "");

    std::string type = j.value("type", "");
    Vec3 size = j.value("size", Vec3(1.f));
    Vec3 sphereData = j.value("sphereData", Vec3{1.0, 16.0, 32.0});

    if (type == "Obj")
      asset = Mesh::loadFromObj(path, uuid);
    else if (type == "Box")
      asset = Mesh::createBox(size[0], size[1], size[2], uuid);
    else if (type == "Sphere")
      asset =
          Mesh::createSphere(sphereData[0], sphereData[1], sphereData[2], uuid);
    else if (type == "Sprite")
      asset = Mesh::createQuad(size[0], uuid);

    am.add(asset, name);

  } else if constexpr (std::is_same_v<T, Texture>) {

    std::string path = j.value("path", "");
    asset = Texture::load(path, uuid);
    am.add(asset, name);

  } else if constexpr (std::is_same_v<T, Material>) {

    asset = std::make_shared<Material>(uuid);
    asset->baseColor = j["baseColor"];
    asset->ambient = j["ambient"];
    asset->specular = j["specular"];
    asset->shininess = j["shininess"];
    asset->diffuse = j["diffuse"];
    asset->texture = j["texture"];

    asset->useTexture = j["useTexture"];
    asset->doubleSided = j["doubleSided"];

    am.add(asset, name);

  } else if constexpr (std::is_same_v<T, Font>) {

    std::string path = j.value("path", "");
    int ptsize = j.value("ptsize", 24);
    asset = Font::loadFont(path, ptsize, uuid);
    am.add(asset, name);
  } else {
    static_assert(false, "Unsupported asset type");
  }

  return asset;
}
} // namespace farixEngine
