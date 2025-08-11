
#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <limits>
#include <vector>

#include "farixEngine/assets/font.hpp"
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
  bool is2DPass = false;
  bool enableZBuffer = true;
  bool enableLighting = true;

  uint32_t clearColor = 0xFF87CEEB;
};

struct MaterialData {
  Vec4 baseColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
  float ambient = 0.1f;
  float diffuse = 0.9f;
  float specular = 0.5f;
  float shininess = 32.0f;
  bool useTexture = false;
  bool doubleSided = true;
  Vec3 uvMin = Vec3(0.0f, 0.0f, 0);
  Vec3 uvMax = Vec3(1.0f, 1.0f, 0);
  Texture *texture = nullptr;
};

struct VertexData {
  Vec3 position;
  Vec3 normal;
  Vec2 uv;
};

struct TriangleData {
  uint32_t i0 = 0;
  uint32_t i1 = 0;
  uint32_t i2 = 0;
};

struct MeshData {
  std::vector<VertexData> vertices;
  std::vector<uint32_t> indices;
};

// struct MeshGPU {
//   uint32_t vao = 0;
//   uint32_t vbo = 0;
//   uint32_t ebo = 0;
//   size_t indexCount = 0;
//   bool isUploaded = false;
//
//   void uploadToGPU(const MeshData &meshData) {
//     if (isUploaded)
//       return;
//
//     glGenVertexArrays(1, &vao);
//     glGenBuffers(1, &vbo);
//     glGenBuffers(1, &ebo);
//
//     glBindVertexArray(vao);
//
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER, meshData.vertices.size() * sizeof(VertexData),
//                  meshData.vertices.data(), GL_STATIC_DRAW);
//
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//                  meshData.triangles.size() * sizeof(TriangleData),
//                  meshData.triangles.data(), GL_STATIC_DRAW);
//
//     constexpr GLsizei stride = sizeof(VertexData);
//
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
//                           (void *)offsetof(VertexData, position));
//
//     glEnableVertexAttribArray(1);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
//                           (void *)offsetof(VertexData, normal));
//
//     glEnableVertexAttribArray(2);
//     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
//                           (void *)offsetof(VertexData, uv));
//
//     glBindVertexArray(0);
//
//     isUploaded = true;
//     indexCount = meshData.triangles.size() * 3;
//   }
//
//   void release() {
//     if (!isUploaded)
//       return;
//     glDeleteBuffers(1, &vbo);
//     glDeleteBuffers(1, &ebo);
//     glDeleteVertexArrays(1, &vao);
//     vao = vbo = ebo = 0;
//     isUploaded = false;
//     indexCount = 0;
//   }
// };

struct ClippableVertex {
  Vec4 cposition;
  Vec3 normal;
  Vec2 uv;
  Vec3 position;

  ClippableVertex lerp(const ClippableVertex &other, float t) const {
    return {cposition + (other.cposition - cposition) * t,
            normal + (other.normal - normal) * t, uv + (other.uv - uv) * t,

            position + (other.position - position) * t};
  }
};
struct UITextDrawCommand {
  Font *font;
  std::string text;
  Vec3 pos;
  float size;
  Vec4 color;
};

struct SpriteData {
  Texture *texture;
  Vec4 color = Vec4(1.0f);
  Vec3 size = Vec3(1.f, 1.f, 0);
  bool flipX = false;
  bool flipY = false;
  Vec3 uvMin = Vec3(0.0f, 0.0f, 0);
  Vec3 uvMax = Vec3(1.0f, 1.0f, 0);

  bool useTexture = false;
};

} // namespace farixEngine::renderer
