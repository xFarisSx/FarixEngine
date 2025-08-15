
#pragma once

#include <SDL2/SDL.h>

#include <cstdint>
#include <vector>

#include "farixEngine/assets/font.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/math/vec4.hpp"
#include "farixEngine/renderer/renderData.hpp"

namespace farixEngine::renderer {

class IRenderer {
public:
  IRenderer(int width, int height, const char *title)
      : screenWidth(width), screenHeight(height) {}

  virtual ~IRenderer() = default;

  virtual void beginFrame(const RenderContext &context) = 0;
  virtual void setContext(const RenderContext &context) = 0;
  virtual void beginUIPass(int screenW, int screenH) = 0;
  virtual RenderContext makeUIContext(int screenW, int screenH) = 0;
  virtual void endFrame() = 0;

  virtual void clear(uint32_t color = 0xFF000000) = 0;
  virtual void present() = 0;

  virtual void renderMesh(const MeshData &mesh, const Mat4 &model,
                          const MaterialData &material) = 0;

  virtual void renderSprite(const SpriteData &sprite, const Mat4 &model) = 0;

  virtual void drawText(Font *font, const std::string &str, Vec3 pos,
                        float size, Vec4 color) = 0;

  virtual std::array<int, 2> getScreenSize() = 0;

protected:
  SDL_Window *window = nullptr;

  int screenWidth = 0;
  int screenHeight = 0;

  RenderContext currentContext;
  std::vector<UITextDrawCommand> textDrawQueue;

  Vec3 lightDir = Vec3(0, 0, -1);
};

} // namespace farixEngine::renderer
