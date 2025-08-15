#pragma once

#include "farixEngine/renderer/renderData.hpp"
#include "farixEngine/renderer/renderer.hpp"
#include <SDL2/SDL.h>
#include "farixEngine/thirdparty/glad/glad.h"

namespace farixEngine::renderer {

class OpenGLRenderer : public IRenderer {
public:
  OpenGLRenderer(int width, int height, const char *title);

  ~OpenGLRenderer() override;

  void beginFrame(const RenderContext &context) override;
  void setContext(const RenderContext &context) override;
  void beginUIPass(int screenW, int screenH) override;
  RenderContext makeUIContext(int screenW, int screenH) override;
  void endFrame() override;

  void clear(uint32_t color = 0xFF000000) override;
  void present() override;

  void renderMesh(const MeshData &mesh, const Mat4 &model,
                  const MaterialData &material) override;

  void renderSprite(const SpriteData &sprite, const Mat4 &model) override;

  void drawText(Font *font, const std::string &str, Vec3 pos, float size,
                Vec4 color) override;

  std::array<int, 2> getScreenSize() override;
};

} // namespace farixEngine::renderer
