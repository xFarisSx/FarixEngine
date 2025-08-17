#pragma once

#include "farixEngine/renderer/opengl/shader.hpp"

#include "farixEngine/renderer/renderData.hpp"
#include "farixEngine/renderer/renderer.hpp"
#include "farixEngine/thirdparty/glad/glad.h"
#include <SDL2/SDL.h>

namespace farixEngine::renderer {

class OpenGLRenderer : public IRenderer {
public:
  OpenGLRenderer(int width, int height, const char *title);

  ~OpenGLRenderer() override;

  void beginFrame() override;
  void beginPass(RenderContext &context) override;
  void setContext(RenderContext &context) override;
  void endPass() override;
  void endFrame() override;

  void clear(uint32_t color = 0xFF87CEEB) override;
  void present() override;

  void submitMesh(const std::shared_ptr<MeshData> mesh, const Mat4 &model,
                  const MaterialData &material) override;

  void submitSprite(const SpriteData &sprite, const Mat4 &model) override;

  void submitText(Font *font, const std::string &str, Vec3 pos, float size,
                  Vec4 color, Mat4 model) override;
  void submitText2D(Font *font, const std::string &str, Vec3 pos, float size,
                    Vec4 color);

  void renderMesh(const MeshCommand &meshCommand) override;
  void renderMesh(const GPUMeshCommand &meshCommand);
  void renderText(const UITextDrawCommand &textCommand) override;

  std::array<int, 2> getScreenSize() override;

  std::shared_ptr<GPUMesh>
  createOrGetGPUMesh(const std::shared_ptr<MeshData> &mesh);
  std::shared_ptr<Texture> createOrGetGPUTexture(::farixEngine::Texture *tex);

private:
  std::unordered_map<std::string, std::shared_ptr<GPUMesh>> gpuMeshCache;
  std::unordered_map<std::string, std::shared_ptr<Texture>> gpuTextureCache;
  Shader defaultShaderProgram;
  Shader textShaderProgram;
};

} // namespace farixEngine::renderer
