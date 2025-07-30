#pragma once
#include "farixEngine/math/vec3.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <vector>

namespace farixEngine {

struct Triangle {
  int i0, i1, i2;
  int uv0, uv1, uv2 = 0;
};

struct Mesh {
  std::vector<Vec3> vertices;
  std::vector<Triangle> triangles;
  std::vector<Vec3> textureMap;
  std::string path;
  std::string type;

  Vec3 size{1};
  Vec3 sphereData{1.0, 16.0, 32.0};

  static std::shared_ptr<Mesh> createBox(float width, float height, float depth);
  static std::shared_ptr<Mesh> createSphere(float radius, int latSegments, int lonSegments);
  static std::shared_ptr<Mesh> createQuad(Vec3 size);
  static std::shared_ptr<Mesh> loadFromObj(const std::string &filename);
};
} // namespace farixEngine
