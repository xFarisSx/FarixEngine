#pragma once
#include "farixEngine/assets/font.hpp"
#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/assets/texture.hpp"

#include "farixEngine/math/mat4.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/utils/uuid.hpp"
#include <functional>
#include <memory>
#include <unordered_map>
namespace farixEngine {
using UUID = std::string;
using AssetID = std::string;
struct Timer {
  float current = 0.0f;
  float max = 1.0f;
  bool repeat = false;
  bool finished = false;
  std::string name = "";
};
class Script;
using ScriptPtr = std::shared_ptr<Script>;
using TimerPtr = std::shared_ptr<Timer>;
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
enum class CameraProjectionMode { Perspective, Orthographic };
struct CameraComponent {
  CameraProjectionMode mode = CameraProjectionMode::Perspective;

  float fov = M_PI / 2;
  float aspectRatio = 16.0f / 9.0f;
  float nearPlane = 1.0f;
  float farPlane = 100.0f;

  float orthoLeft = -1.0f;
  float orthoRight = 1.0f;
  float orthoBottom = -1.0f;
  float orthoTop = 1.0f;
  float orthoNear = 0;
  float orthoFar = 100.0f;

  void setOrthoZoom(float zoom) {
    float aspect = aspectRatio;
    orthoLeft = -aspect * zoom;
    orthoRight = aspect * zoom;
    orthoBottom = -zoom;
    orthoTop = zoom;
  }
};
struct CameraControllerComponent {
  float sens = 0.5f;
  float speed = 5.f;
  bool active = true;
};

struct MeshComponent {

  AssetID mesh;
};

struct BillboardComponent {
  enum class BillboardType {
    None,
    BillboardY,
    BillboardFull,
  };
  BillboardType type = BillboardType::BillboardY;
};
struct MaterialComponent {
  Vec4 baseColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
  float ambient = 0.1f;
  float specular = 0.5f;
  float shininess = 32.0f;
  float diffuse = 1.0f;
  AssetID texture;

  bool useTexture = false;
  bool doubleSided = true;
};

struct ScriptComponent { 
  std::vector<ScriptPtr> scripts;
};

struct ParentComponent {
  Entity parent;
};

struct ChildrenComponent {
  std::vector<Entity> children;
};

struct RigidBodyComponent {
  Vec3 velocity = Vec3(0.0f);
  Vec3 acceleration = Vec3(0.0f);
  float mass = 1.0f;
  bool isKinematic = false;
};

struct ColliderComponent {
  enum class Shape { Box, Sphere, Capsule };

  Shape shape = Shape::Box;
  Vec3 size = Vec3(1.0f);
  float radius = 1.0f;
  bool isTrigger = false;
};

struct VariableComponent {
  std::unordered_map<std::string, float> floats;
  std::unordered_map<std::string, int> ints;
  std::unordered_map<std::string, std::string> strings;
};

struct StateComponent {
  std::string currentState;
  std::unordered_map<std::string, std::string> transitions;
};

struct LifetimeComponent {
  float timeRemaining = 1.0f;
};

struct AudioSourceComponent {
  std::string soundPath;
  bool loop = false;
  float volume = 1.0f;
};

struct LightComponent {
  enum class Light { Point, Directional, Spot };
  Light type = Light::Point;
  Vec3 color = Vec3(1.0f);
  Vec3 dir = Vec3(0);
  float intensity = 1.0f;
  float range = 10.0f;
  float spotAngle = 45.0f;
};

struct TimersComponent {
  std::unordered_map<std::string, TimerPtr> timers;
};

struct Sprite2DComponent {
  AssetID texture;
  Vec4 color = Vec4(1.0f);
  Vec3 size = Vec3(1.f, 1.f, 0);
  bool flipX = false;
  bool flipY = false;

  bool useTexture = false;
};

struct UIComponent {
  enum class Anchor {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Center,
    TopCenter,
    BottomCenter,
    LeftCenter,
    RightCenter
  }; 
  Anchor anchor = Anchor::TopLeft;
  bool interactable = false;
  bool blockRaycast = true;
};

struct RectComponent {
  Vec3 position = Vec3(0);
  Vec3 size = Vec3(100, 50, 0);
  float rotation = 0.0f;
};

struct UIImageComponent {
  AssetID texture;
  Vec4 color = Vec4(1.0f);
  bool useTexture = false;
};
struct UITextComponent {
  std::string text;
  Vec4 color = Vec4(1.0f);
  float fontSize = 16.0f;
  AssetID font;
};

struct UIButtonComponent {
  std::function<void()> onClick;
  bool isHovered = false;
  bool isPressed = false;
};

} // namespace farixEngine
