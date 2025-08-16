
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

  virtual void beginFrame() = 0;
  virtual void beginPass(RenderContext &context) = 0;
  virtual void setContext(RenderContext &context) = 0;
  virtual void endPass() = 0;
  virtual void endFrame() = 0;

  virtual void clear(uint32_t color = 0xFF87CEEB) = 0;
  virtual void present() = 0;
  virtual void submitMesh(const std::shared_ptr<MeshData> mesh,
                          const Mat4 &model, const MaterialData &material) = 0;

  virtual void submitSprite(const SpriteData &sprite, const Mat4 &model) = 0;

  virtual void submitText(Font *font, const std::string &str, Vec3 pos,
                          float size, Vec4 color) = 0;

  virtual void renderMesh(const MeshCommand &meshCommand) = 0;
  virtual void renderText(const UITextDrawCommand &textCommand) = 0;

  virtual std::array<int, 2> getScreenSize() = 0;

  Vec4 unpackColor(uint32_t color);
  uint32_t packColor(const Vec4 &color);
  std::shared_ptr<MeshData> quadMesh2D() ;

protected:
  SDL_Window *window = nullptr;

  int screenWidth = 0;
  int screenHeight = 0;

  std::vector<RenderPass> passes;
  RenderPass *activePass = nullptr;
  RenderContext *currentContext = nullptr;

  Vec3 lightDir = Vec3(0, 0, -1);
};

} // namespace farixEngine::renderer
