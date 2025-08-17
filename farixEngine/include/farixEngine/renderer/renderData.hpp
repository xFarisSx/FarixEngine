
#pragma once

#include "farixEngine/thirdparty/glad/glad.h"
#include <SDL2/SDL.h>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include "farixEngine/assets/font.hpp"
#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/assets/texture.hpp"
#include "farixEngine/math/mat4.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/math/vec4.hpp"
#include "farixEngine/renderer/opengl/EBO.hpp"
#include "farixEngine/renderer/opengl/VAO.hpp"
#include "farixEngine/renderer/opengl/VBO.hpp"
#include "farixEngine/renderer/opengl/texture.hpp"

namespace farixEngine::renderer {
struct RenderContext {
  Mat4 viewMatrix;
  Mat4 projectionMatrix;
  Vec3 cameraPosition;
  bool isOrthographic = false;
  bool is2DPass = false;
  bool enableZBuffer = true;
  bool enableLighting = true;
  Vec4 lightColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
  Vec3 lightPos = Vec3(0, 0, -5);
  float nearPlane = 0.1f;
  float farPlane = 100.0f;

  uint32_t clearColor = 0xFF87CEEB;
};

struct MaterialData {
  std::string uuid = "";
  Vec4 baseColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
  float ambient = 0.1f;
  float diffuse = 0.9f;
  float specular = 0.5f;
  float shininess = 32.0f;
  bool useTexture = false;
  bool doubleSided = true;
  Vec2 uvMin = Vec2(0.0f, 0.0f);
  Vec2 uvMax = Vec2(1.0f, 1.0f);
  farixEngine::Texture *texture = nullptr;
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
  std::string uuid = "";
  std::vector<VertexData> vertices;
  std::vector<uint32_t> indices;
};

struct MeshCommand {
  std::shared_ptr<MeshData> meshData;
  MaterialData matData;
  Mat4 modelMatrix;
};
struct UITextDrawCommand {

  Font *font;
  std::string text;
  Vec3 pos;
  float size;
  Vec4 color;
  std::string uuid = "";
};



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
struct SpriteData {
  farixEngine::Texture *texture;
  Vec4 color = Vec4(1.0f);
  Vec3 size = Vec3(1.f, 1.f, 0);
  bool flipX = false;
  bool flipY = false;
  Vec2 uvMin = Vec2(0.0f, 0.0f);
  Vec2 uvMax = Vec2(1.0f, 1.0f);

  bool useTexture = false;
};

struct GPUMesh {
  VAO vao;
  VBO vbo;
  EBO ebo;
  size_t indexCount;


  GPUMesh(MeshData &mesh) {
    vao.Bind();

    vbo = VBO((GLfloat *)mesh.vertices.data(),
              mesh.vertices.size() * sizeof(VertexData));

    ebo = EBO(mesh.indices.data(), mesh.indices.size() * sizeof(uint32_t));

    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(VertexData),
                   (void *)offsetof(VertexData, position));
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(VertexData),
                   (void *)offsetof(VertexData, normal));
    vao.LinkAttrib(vbo, 2, 2, GL_FLOAT, sizeof(VertexData),
                   (void *)offsetof(VertexData, uv));

    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind(); 

    indexCount = mesh.indices.size();
  }
};

struct GPUMaterialData{
  std::string uuid = "";
  Vec4 baseColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
  float ambient = 0.1f;
  float diffuse = 0.9f;
  float specular = 0.5f;
  float shininess = 32.0f;
  bool useTexture = false;
  bool doubleSided = true;
  Vec2 uvMin = Vec2(0.0f, 0.0f);
  Vec2 uvMax = Vec2(1.0f, 1.0f); 
  std::shared_ptr<renderer::Texture> texture = nullptr;

} ;


struct GPUMeshCommand{
  std::shared_ptr<GPUMesh> gpuMesh;
  GPUMaterialData gpuMatData;
  Mat4 modelMatrix;
};

struct RenderPass {
  RenderContext context;
  std::vector<MeshCommand> meshCommands;
  std::vector<UITextDrawCommand> textCommands;
  std::vector<GPUMeshCommand> gpuMeshCommands;
};

} // namespace farixEngine::renderer
