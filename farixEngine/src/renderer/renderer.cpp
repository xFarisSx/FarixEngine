#include "farixEngine/renderer/renderer.hpp"
#include "farixEngine/renderer/renderData.hpp"
#include <SDL2/SDL.h>
#include "farixEngine/thirdparty/glad/glad.h"
#include "algorithm"

namespace farixEngine::renderer{

Vec4 IRenderer::unpackColor(uint32_t color) {
  uint8_t a = (color >> 24) & 0xFF;
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  return Vec4(r, g, b, a) / 255.0f;
}

uint32_t IRenderer::packColor(const Vec4 &color) {
  uint8_t a = static_cast<uint8_t>(std::clamp(color.w, 0.f, 1.f) * 255.f);
  uint8_t r = static_cast<uint8_t>(std::clamp(color.x, 0.f, 1.f) * 255.f);
  uint8_t g = static_cast<uint8_t>(std::clamp(color.y, 0.f, 1.f) * 255.f);
  uint8_t b = static_cast<uint8_t>(std::clamp(color.z, 0.f, 1.f) * 255.f);
  return (a << 24) | (r << 16) | (g << 8) | b;
}

std::shared_ptr<MeshData> IRenderer::quadMesh2D() {
  static std::shared_ptr<MeshData> quad;

  if (!quad) {
    quad = std::make_shared<MeshData>();
    quad->uuid = utils::generateUUID(); 

    float w = 0.5f, h = 0.5f;

    quad->vertices = {{Vec3(-w, -h, 0), Vec3(0, 0, 1), Vec2(0, 0)},
                      {Vec3(w, -h, 0), Vec3(0, 0, 1), Vec2(1, 0)},
                      {Vec3(w, h, 0), Vec3(0, 0, 1), Vec2(1, 1)},
                      {Vec3(-w, h, 0), Vec3(0, 0, 1), Vec2(0, 1)}};

    quad->indices = {0, 1, 2, 0, 2, 3};
  }

  return quad;
}

} 
