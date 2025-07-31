
#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <limits>
#include <vector>

#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/assets/texture.hpp"
#include "farixEngine/math/mat4.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/math/vec4.hpp"

namespace farixEngine::renderer {
struct RenderContext {
  Mat4 viewMatrix;
  Mat4 projectionMatrix;
  Vec3 cameraPosition;
  bool isOrthographic = false;

  uint32_t clearColor = 0xFF87CEEB;
};

struct MaterialData {
  Vec3 baseColor = Vec3(1.0f, 1.0f, 1.0f);
  float ambient = 0.1f;
  float diffuse = 0.9f;
  float specular = 0.5f;
  float shininess = 32.0f;
  bool useTexture = false;
  bool doubleSided = true;
  Texture *texture = nullptr;
};

struct TriangleData {
  int i0, i1, i2;
  int uv0, uv1, uv2 = 0;
    int n0, n1, n2 = 0;
};

struct MeshData {
  std::vector<Vec3> positions;
  std::vector<Vec3> normals;
  std::vector<Vec3> uvs;
  std::vector<TriangleData> indices;
};

struct SpriteData {
  Texture* texture;
  Vec4 color = Vec4(1.0f);
  Vec3 size = Vec3(1.f, 1.f, 0);
  bool flipX = false;
  bool flipY = false;

  bool useTexture = false;
};

} // namespace farixEngine::renderer
