
#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <vector>

#include "farixEngine/assets/font.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/math/vec4.hpp"
#include "farixEngine/renderer/helpers.hpp"

namespace farixEngine::renderer {

class Renderer {
public:
  Renderer(int width, int height, const char *title);
  ~Renderer();

  void beginFrame(const RenderContext &context);
  void setContext(const RenderContext &context);
  void beginUIPass(int screenW, int screenH);
  RenderContext makeUIContext(int screenW, int screenH);
  void endFrame();

  void clear(uint32_t color = 0xFF000000);
  void present();

  uint32_t packColor(const Vec4 &color);
  Vec4 unpackColor(uint32_t color);

  Vec4 project(const Vec4 &point, const Mat4 &model,
               const RenderContext &ctx) const;

  void drawPixel(int x, int y, float z, uint32_t color);

  float edgeFunction(const Vec4 &a, const Vec4 &b, const Vec4 &c) const;
  bool isTriangleValid(const Vec4 &p0, const Vec4 &p1, const Vec4 &p2) const;

  Vec3 reflect(const Vec3 &L, const Vec3 &N);
  std::array<Vec4, 3> fetchTransformedVertices(const MeshData &mesh,
                                               const TriangleData &tri,
                                               const Mat4 &model,
                                               const RenderContext &ctx) const;
  bool isTriangleVisible(std::array<Vec4, 3> &projected,
                         const MaterialData &material,
                         const RenderContext &ctx) const;
  std::array<Vec3, 3> fetchUVs(const MeshData &mesh, const TriangleData &tri,
                               const MaterialData &material) const;
  std::array<Vec3, 3> fetchNs(const MeshData &mesh, const TriangleData &tri,
                              const MaterialData &material) const;
  std::array<Vec3, 3> fetchPs(const MeshData &mesh, const TriangleData &tri,
                              const MaterialData &material) const;

  Vec4 shadeFragment(const Vec3 &worldPos, const Vec3 &uv, const Vec3 &normal,
                     const RenderContext &ctx, const MaterialData &material);

  void rasterizeTriangle(const std::array<Vec4, 3> &projected,
                         const std::array<Vec3, 3> &ps,
                         const std::array<Vec3, 3> &uvs,
                         const std::array<Vec3, 3> &ns,
                         const RenderContext &ctx,
                         const MaterialData &material);

  Vec4 toCameraSpace(const Vec3 &pos, const Mat4 &model, const Mat4 &view);
  Vec4 projectToClipSpace(const Vec4 &posCamera, const Mat4 &proj);
  Vec4 ndcToScreen(Vec4 &posClip, int screenWidth, int screenHeight);
  bool isTriangleVisibleInFrustum(const Vec4 &v0, const Vec4 &v1,
                                  const Vec4 &v2);
  std::vector<std::vector<ClippableVertex>>
  clipTriangleAgainstNearPlane(const ClippableVertex &v0,
                               const ClippableVertex &v1,
                               const ClippableVertex &v2);

  void drawTriangle(const MeshData &mesh, const TriangleData &tri,
                    const Mat4 &model, const RenderContext &ctx,
                    const MaterialData &material);

  void renderMesh(const MeshData &mesh, const Mat4 &model,
                  const MaterialData &material);

  void renderSprite(const SpriteData &sprite, const Mat4 &model);
  MeshData quadMesh2D();
  void drawText(Font *font, const std::string &str, Vec3 pos, float size,
                Vec4 color);
  void flushTextDraws();

  std::array<int, 2> getScreenSize();

private:
  SDL_Window *window = nullptr;
  SDL_Renderer *sdlRenderer = nullptr;
  SDL_Texture *sdlTexture = nullptr;

  int screenWidth = 0;
  int screenHeight = 0;

  uint32_t *framebuffer = nullptr;
  std::vector<float> zBuffer;

  RenderContext currentContext;
  std::vector<UITextDrawCommand> textDrawQueue;

  Vec3 lightDir = Vec3(0, 0, -1);
};

} // namespace farixEngine::renderer
