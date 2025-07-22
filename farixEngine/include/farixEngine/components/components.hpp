#pragma once
#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/assets/texture.hpp"

#include "farixEngine/math/mat4.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/utils/uuid.hpp"
#include <memory>

namespace farixEngine {

class Script;
using ScriptPtr = std::shared_ptr<Script>;
using Entity = uint32_t;
struct TransformComponent {
  Vec3 position = {0, 0, 0};
  Vec3 rotation = {0, 0, 0};
  Vec3 scale = {1.0f, 1.0f, 1.0f};
};

struct GlobalTransform {
  Mat4 worldMatrix{};
};

struct Metadata {
  std::string name = "Entity";
  std::vector<std::string> tags;

  std::string uuid = utils::generateUUID();
  std::string prefab;
};

struct CameraComponent {

  float fov = M_PI / 2;
  float aspectRatio = 16.0f / 9.0f;
  float nearPlane = 1.0f;
  float farPlane = 100.0f;
};

struct CameraControllerComponent {
  float sens = 0.5f;
  float speed = 5.f;
  bool active = true;
};

struct MeshComponent {

  std::shared_ptr<Mesh> mesh;
};

struct MaterialComponent {
  Vec3 baseColor = Vec3(1.0f, 1.0f, 1.0f);
  float ambient = 0.1f;
  float specular = 0.5f;
  float shininess = 32.0f;
  std::shared_ptr<Texture> texture;

  bool useTexture = false;
};

struct ScriptComponent {
  ScriptPtr script;
};

struct ParentComponent {
  Entity parent;
};

struct ChildrenComponent {
  std::vector<Entity> children;
};

} // namespace farixEngine
