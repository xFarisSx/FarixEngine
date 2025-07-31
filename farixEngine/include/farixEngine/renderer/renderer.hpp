
#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <vector>

#include "farixEngine/math/vec3.hpp"
#include "farixEngine/math/vec4.hpp"
#include "farixEngine/renderer/helpers.hpp"

namespace farixEngine::renderer {

class Renderer {
public:
  Renderer(int width, int height, const char *title);
  ~Renderer();

  void beginFrame(const RenderContext &context);
  void endFrame();

  void clear(uint32_t color = 0xFF000000);
  void present();

  uint32_t packColor(const Vec3 &color);
  Vec3 unpackColor(uint32_t color);

  Vec4 project(const Vec4 &point, const Mat4 &model,
               const RenderContext &ctx) const;

  void drawPixel(int x, int y, float z, uint32_t color);

  float edgeFunction(const Vec4 &a, const Vec4 &b, const Vec4 &c) const;
  bool isTriangleValid(const Vec4 &p0, const Vec4 &p1, const Vec4 &p2) const;

  Vec3 reflect(const Vec3 &L, const Vec3 &N);
  std::array<Vec4, 3>
  fetchTransformedVertices(const MeshData &mesh,
                                   const TriangleData &tri, const Mat4 &model,
                                   const RenderContext &ctx) const ;
  bool isTriangleVisible(std::array<Vec4, 3> &projected,
                                 const MaterialData &material,
                                 const RenderContext &ctx) const;
  std::array<Vec3, 3>fetchUVs(const MeshData &mesh,
                                       const TriangleData &tri,
                                       const MaterialData &material) const ;

  Vec3 shadeFragment(const Vec3 &worldPos, const Vec3 &uv, const Vec3 &normal,
                     const RenderContext &ctx, const MaterialData &material);

  void rasterizeTriangle(const std::array<Vec4, 3> &projected,
                                 const std::array<Vec3, 3> &uvs,
                                 const MeshData &mesh, const TriangleData &tri,
                                 const RenderContext &ctx,
                                 const MaterialData &material) ;

  void drawTriangle(const MeshData &mesh, const TriangleData &tri,
                    const Mat4 &model, const RenderContext &ctx,
                    const MaterialData &material);

  void renderMesh(const MeshData &mesh, const Mat4 &model,
                  const MaterialData &material);

  void renderSprite(const SpriteData &sprite, const Mat4 &model);
  MeshData quadMesh2D();

private:
  SDL_Window *window = nullptr;
  SDL_Renderer *sdlRenderer = nullptr;
  SDL_Texture *sdlTexture = nullptr;

  int screenWidth = 0;
  int screenHeight = 0;

  uint32_t *framebuffer = nullptr;
  std::vector<float> zBuffer;

  RenderContext currentContext;
  Vec3 lightDir = Vec3(0, 0, -1);
};

} // namespace farixEngine::renderer
