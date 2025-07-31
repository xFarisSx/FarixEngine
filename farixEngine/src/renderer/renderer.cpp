
#include "farixEngine/renderer/renderer.hpp"
#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/components/components.hpp"
#include "farixEngine/core/world.hpp"
#include "farixEngine/math/general.hpp"
#include "farixEngine/math/mat4.hpp"
#include "farixEngine/math/vec4.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

namespace farixEngine::renderer {

Renderer::Renderer(int width, int height, const char *title)
    : screenWidth(width), screenHeight(height) {

  window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, height, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Window Error: " << SDL_GetError() << "\n";
    exit(1);
  }

  sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!sdlRenderer) {
    std::cerr << "Renderer Error: " << SDL_GetError() << "\n";
    exit(1);
  }

  sdlTexture =
      SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);

  framebuffer = new uint32_t[screenWidth * screenHeight];
  zBuffer.resize(screenWidth * screenHeight);
}

Renderer::~Renderer() {
  delete[] framebuffer;
  SDL_DestroyTexture(sdlTexture);
  SDL_DestroyRenderer(sdlRenderer);
  SDL_DestroyWindow(window);
}

void Renderer::beginFrame(const RenderContext &context) {
  currentContext = context;
  clear(context.clearColor);
}
void Renderer::endFrame() { present(); }

void Renderer::clear(uint32_t color) {
  if (!framebuffer) {
    std::cerr << "Error: framebuffer is null!" << std::endl;
    return;
  }
  if (screenWidth == 0 || screenHeight == 0) {
    std::cerr << "Error: screenWidth or screenHeight is zero!" << std::endl;
    return;
  }

  std::fill(framebuffer, framebuffer + screenWidth * screenHeight, color);
  std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::max());
}

void Renderer::present() {
  SDL_UpdateTexture(sdlTexture, nullptr, framebuffer,
                    screenWidth * sizeof(uint32_t));
  SDL_RenderClear(sdlRenderer);
  SDL_RenderCopy(sdlRenderer, sdlTexture, nullptr, nullptr);
  SDL_RenderPresent(sdlRenderer);
}

uint32_t Renderer::packColor(const Vec3 &color) {
  uint8_t a = 255;
  uint8_t r = static_cast<uint8_t>(std::clamp(color.x, 0.f, 1.f) * 255.f);
  uint8_t g = static_cast<uint8_t>(std::clamp(color.y, 0.f, 1.f) * 255.f);
  uint8_t b = static_cast<uint8_t>(std::clamp(color.z, 0.f, 1.f) * 255.f);
  return (a << 24) | (r << 16) | (g << 8) | b;
}

Vec3 Renderer::unpackColor(uint32_t color) {
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  return Vec3(r, g, b) / 255.0f;
}

Vec4 Renderer::project(const Vec4 &point, const Mat4 &model,
                       const RenderContext &ctx) const {
  Vec4 projected4 = ctx.projectionMatrix * ctx.viewMatrix * model * point;
  if (projected4.w <= 0.0f)
    return Vec3(-1, -1, -1);
  Vec3 pr = projected4.toVec3();
  return Vec4(screenWidth * (pr.x + 1) / 2, screenHeight * (1 - pr.y) / 2, pr.z,
              projected4.w);
}

void Renderer::drawPixel(int x, int y, float z, uint32_t color) {
  if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
    int index = y * screenWidth + x;
    if (z < zBuffer[index]) {
      zBuffer[index] = z;
      framebuffer[index] = color;
    }
  }
}

Vec3 Renderer::reflect(const Vec3 &L, const Vec3 &N) {
  return L - N * (2.0f * L.dot(N));
}

float Renderer::edgeFunction(const Vec4 &a, const Vec4 &b,
                             const Vec4 &c) const {
  Vec4 v1 = b - a;
  Vec4 v2 = c - a;
  return v1.x * v2.y - v1.y * v2.x;
}

bool Renderer::isTriangleValid(const Vec4 &p0, const Vec4 &p1,
                               const Vec4 &p2) const {
  auto isValid = [](const Vec4 &v) {
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
  };
  return isValid(p0) && isValid(p1) && isValid(p2) && p0.z >= 0 && p0.z <= 1 &&
         p1.z >= 0 && p1.z <= 1 && p2.z >= 0 && p2.z <= 1;
}

std::array<Vec4, 3>
Renderer::fetchTransformedVertices(const MeshData &mesh,
                                   const TriangleData &tri, const Mat4 &model,
                                   const RenderContext &ctx) const {

  Vec4 v0 = project(Vec4(mesh.positions[tri.i0], 1), model, ctx);
  Vec4 v1 = project(Vec4(mesh.positions[tri.i1], 1), model, ctx);
  Vec4 v2 = project(Vec4(mesh.positions[tri.i2], 1), model, ctx);
  return {v0, v1, v2};
}

bool Renderer::isTriangleVisible(std::array<Vec4, 3> &projected,
                                 const MaterialData &material,
                                 const RenderContext &ctx) const {

  if (!isTriangleValid(projected[0], projected[1], projected[2]))
    return false;

  if (material.doubleSided)
    return true;

  Vec3 normal = (projected[1].toVec3() - projected[0].toVec3())
                    .cross(projected[2].toVec3() - projected[0].toVec3())
                    .normalized();

  Vec3 viewDir = (ctx.viewMatrix.getCol(2).xyz().normalized()) * -1;
  return normal.dot(viewDir) < 0.2f;
}

std::array<Vec3, 3> Renderer::fetchUVs(const MeshData &mesh,
                                       const TriangleData &tri,
                                       const MaterialData &material) const {

  if (material.useTexture && !mesh.uvs.empty()) {
    return {mesh.uvs[tri.uv0], mesh.uvs[tri.uv1], mesh.uvs[tri.uv2]};
  }
  return {Vec3(), Vec3(), Vec3()};
}

void Renderer::rasterizeTriangle(const std::array<Vec4, 3> &projected,
                                 const std::array<Vec3, 3> &uvs,
                                 const MeshData &mesh, const TriangleData &tri,
                                 const RenderContext &ctx,
                                 const MaterialData &material) {

  float minX = std::min({projected[0].x, projected[1].x, projected[2].x});
  float maxX = std::max({projected[0].x, projected[1].x, projected[2].x});
  float minY = std::min({projected[0].y, projected[1].y, projected[2].y});
  float maxY = std::max({projected[0].y, projected[1].y, projected[2].y});

  int minXInt = std::max(0, (int)std::floor(minX));
  int maxXInt = std::min(screenWidth - 1, (int)std::ceil(maxX));
  int minYInt = std::max(0, (int)std::floor(minY));
  int maxYInt = std::min(screenHeight - 1, (int)std::ceil(maxY));

  float area = edgeFunction(projected[0], projected[1], projected[2]);
  if (std::abs(area) < 1e-6f)
    return;

  for (int y = minYInt; y <= maxYInt; ++y) {
    for (int x = minXInt; x <= maxXInt; ++x) {
      Vec3 p(x, y, 0);
      float w0 = edgeFunction(projected[1], projected[2], p);
      float w1 = edgeFunction(projected[2], projected[0], p);
      float w2 = edgeFunction(projected[0], projected[1], p);

      if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
        float alpha = w0 / area, beta = w1 / area, gamma = w2 / area;
        float depth = alpha * projected[0].z + beta * projected[1].z +
                      gamma * projected[2].z;
        if (!std::isfinite(depth) || depth < 0 || depth > 1)
          continue;

        Vec3 worldPos = mesh.positions[tri.i0] * alpha +
                        mesh.positions[tri.i1] * beta +
                        mesh.positions[tri.i2] * gamma;

        float invW = alpha / projected[0].w + beta / projected[1].w +
                     gamma / projected[2].w;
        float u = (alpha * uvs[0].x / projected[0].w +
                   beta * uvs[1].x / projected[1].w +
                   gamma * uvs[2].x / projected[2].w) /
                  invW;
        float v = (alpha * uvs[0].y / projected[0].w +
                   beta * uvs[1].y / projected[1].w +
                   gamma * uvs[2].y / projected[2].w) /
                  invW;

        Vec3 n0 = mesh.normals[tri.n0];
        Vec3 n1 = mesh.normals[tri.n1];
        Vec3 n2 = mesh.normals[tri.n2];
        Vec3 interpolatedNormal =
            (n0 * alpha + n1 * beta + n2 * gamma).normalized();

        Vec3 finalColor = shadeFragment(worldPos, Vec3(u, v, 0),
                                        interpolatedNormal, ctx, material);

        drawPixel(x, y, depth, packColor(finalColor));
      }
    }
  }
}

Vec3 Renderer::shadeFragment(const Vec3 &worldPos, const Vec3 &uv,
                             const Vec3 &normal, const RenderContext &ctx,
                             const MaterialData &material) {
  Vec3 finalColor = material.baseColor;
  if (material.useTexture && material.texture) {
    uint32_t texColor = material.texture->sample(uv.x, uv.y);
    finalColor = unpackColor(texColor);
  }

  Vec3 lightDirView =
      (ctx.viewMatrix * Vec4(lightDir, 0)).toVec3().normalized();
  Vec3 normalView = (ctx.viewMatrix * Vec4(normal, 0)).toVec3().normalized();
  Vec3 viewDir = (ctx.cameraPosition - worldPos).normalized();

  float diff = std::max(material.ambient, normalView.dot(lightDirView * -1));
  Vec3 reflectDir = reflect(lightDirView * -1, normalView);
  float spec =
      std::pow(std::max(0.0f, viewDir.dot(reflectDir)), material.shininess);

  return finalColor * (diff + material.specular * spec);
}

void Renderer::drawTriangle(const MeshData &mesh, const TriangleData &tri,
                            const Mat4 &model, const RenderContext &ctx,
                            const MaterialData &material) {
  auto vertices = fetchTransformedVertices(mesh, tri, model, ctx);
  if (!isTriangleVisible(vertices, material, ctx))
    return;

  auto uvs = fetchUVs(mesh, tri, material);
  rasterizeTriangle(vertices, uvs, mesh, tri, ctx, material);
}

void Renderer::renderMesh(const MeshData &mesh, const Mat4 &model,
                          const MaterialData &material) {
  for (const TriangleData &tri : mesh.indices) {
    drawTriangle(mesh, tri, model, currentContext, material);
  }
}

void Renderer::renderSprite(const SpriteData &sprite, const Mat4 &model) {

  MeshData quadMesh = quadMesh2D();
  MaterialData mat;
  mat.useTexture = sprite.useTexture;
  mat.texture = sprite.texture;
  mat.baseColor = Vec3(1, 1, 1);
  mat.doubleSided = true;

  Mat4 scaleMat = Mat4::scale(Vec3(sprite.size[0], sprite.size[1], 1.0f));
  Mat4 finalMat = model * scaleMat;

  renderMesh(quadMesh, finalMat, mat);
}

MeshData Renderer::quadMesh2D() {
  static MeshData quad;

  if (quad.positions.empty()) {
    float hw = 0.5f, hh = 0.5f;

    quad.positions = {Vec3(-hw, -hh, 0), Vec3(hw, -hh, 0), Vec3(hw, hh, 0),
                      Vec3(-hw, hh, 0)};

    quad.uvs = {Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(1, 1, 0), Vec3(0, 1, 0)};

    quad.normals = {Vec3(0, 0, 1)};

    quad.indices = {{0, 1, 2, 0, 1, 2, 0, 0, 0}, {0, 2, 3, 0, 2, 3, 0, 0, 0}};
  }

  return quad;
}

} // namespace farixEngine::renderer
