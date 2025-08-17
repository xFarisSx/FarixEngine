#pragma once
#include "farixEngine/math/vec2.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/assets/assetManager.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <vector>

namespace farixEngine {

struct Triangle {
  uint32_t i0, i1, i2, n0, n1, n2, uv0, uv1, uv2;
};

struct Vertex {
  Vec3 position;
  Vec3 normal;
  Vec2 uv;

  bool operator==(const Vertex &other) const {
    return position == other.position && normal == other.normal &&
           uv == other.uv;
  }
};

struct Mesh : public Asset {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::string path="";
  std::string type="";
  std::string id="";

  Vec3 size{1};
  Vec3 sphereData{1.0, 16.0, 32.0};

  static std::shared_ptr<Mesh> createBox(float width, float height,
                                         float depth, std::string eid="");
  static std::shared_ptr<Mesh> createSphere(float radius, int latSegments,
                                            int lonSegments, std::string eid="");
  static std::shared_ptr<Mesh> createQuad(Vec3 size, std::string eid="");
  static std::shared_ptr<Mesh> loadFromObj(const std::string &filename, std::string eid="");
  static std::shared_ptr<Mesh> load(const std::string&filename,  std::string eid="");
};
} // namespace farixEngine
//
namespace std {
template <> struct hash<farixEngine::Vertex> {
  size_t operator()(const farixEngine::Vertex &v) const {
    size_t h1 = std::hash<float>()(v.position.x) ^
                (std::hash<float>()(v.position.y) << 1) ^
                (std::hash<float>()(v.position.z) << 2);
    size_t h2 = std::hash<float>()(v.normal.x) ^
                (std::hash<float>()(v.normal.y) << 1) ^
                (std::hash<float>()(v.normal.z) << 2);
    size_t h3 = std::hash<float>()(v.uv.x) ^ (std::hash<float>()(v.uv.y) << 1);

    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};
} // namespace std
