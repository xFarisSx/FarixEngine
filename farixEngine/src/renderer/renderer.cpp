
#include "farixEngine/renderer/renderer.hpp"
#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/components/components.hpp"
#include "farixEngine/core/world.hpp"
#include "farixEngine/math/general.hpp"
#include "farixEngine/math/mat4.hpp"
#include "farixEngine/math/vec4.hpp"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <algorithm>
#include <array>
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
  if (TTF_Init() == -1) {
    std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError()
              << std::endl;
    exit(1);
  }

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
void Renderer::setContext(const RenderContext &context) {
  currentContext = context;
}

RenderContext Renderer::makeUIContext(int screenW, int screenH) {
  RenderContext ctx;
  ctx.viewMatrix = Mat4::identity();
  ctx.projectionMatrix = Mat4::ortho(0, screenW, screenH, 0, 0, 1);
  ctx.enableZBuffer = false;
  ctx.enableLighting = false;
  ctx.is2DPass = true;
  ctx.cameraPosition = Vec3(0);
  return ctx;
}

void Renderer::beginUIPass(int screenW, int screenH) {
  RenderContext uiCtx = makeUIContext(screenW, screenH);
  currentContext = uiCtx;
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
  flushTextDraws();
  SDL_RenderPresent(sdlRenderer);
}

Vec4 Renderer::unpackColor(uint32_t color) {
  uint8_t a = (color >> 24) & 0xFF;
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  return Vec4(r, g, b, a) / 255.0f;
}

uint32_t Renderer::packColor(const Vec4 &color) {
  uint8_t a = static_cast<uint8_t>(std::clamp(color.w, 0.f, 1.f) * 255.f);
  uint8_t r = static_cast<uint8_t>(std::clamp(color.x, 0.f, 1.f) * 255.f);
  uint8_t g = static_cast<uint8_t>(std::clamp(color.y, 0.f, 1.f) * 255.f);
  uint8_t b = static_cast<uint8_t>(std::clamp(color.z, 0.f, 1.f) * 255.f);
  return (a << 24) | (r << 16) | (g << 8) | b;
}

Vec4 Renderer::project(const Vec4 &point, const Mat4 &model,
                       const RenderContext &ctx) const {
  Vec4 projected4 = ctx.projectionMatrix * ctx.viewMatrix * model * point;
  if (projected4.w <= 0.0f)
    return Vec4(-1, -1, -1, -1);
  Vec3 pr = projected4.toVec3();
  return Vec4(screenWidth * (pr.x + 1) / 2, screenHeight * (1 - pr.y) / 2, pr.z,
              projected4.w);
}

void Renderer::drawPixel(int x, int y, float z, uint32_t color) {
  if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
    int index = y * screenWidth + x;
    if (currentContext.enableZBuffer && z >= zBuffer[index])
      return;
    zBuffer[index] = z;

    Vec4 src = unpackColor(color);
    Vec4 dst = unpackColor(framebuffer[index]);
    float alpha = src.w;

    Vec3 outRGB = src.xyz() * alpha + dst.xyz() * (1.0f - alpha);
    float outA = alpha + dst.w * (1.0f - alpha);

    Vec4 outColor(std::clamp(outRGB.x, 0.f, 1.f),
                  std::clamp(outRGB.y, 0.f, 1.f),
                  std::clamp(outRGB.z, 0.f, 1.f), std::clamp(outA, 0.f, 1.f));

    framebuffer[index] = packColor(outColor);
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
         p1.z >= 0 && p1.z <= 1 && p2.z >= 0 && p2.z <= 1 && p0.w > 0 &&
         p1.w > 0 && p2.w > 0;
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
Vec4 Renderer::toCameraSpace(const Vec3 &pos, const Mat4 &model,
                             const Mat4 &view) {
  return view * model * Vec4(pos, 1.0f);
}
Vec4 Renderer::projectToClipSpace(const Vec4 &posCamera, const Mat4 &proj) {
  return proj * posCamera;
}
Vec4 Renderer::ndcToScreen(Vec4 &posClip, int screenWidth, int screenHeight) {
  Vec3 ndc = posClip.xyz() / posClip.w;
  float x = (ndc.x + 1.0f) * 0.5f * screenWidth;
  float y = (1.0f - ndc.y) * 0.5f * screenHeight;
  float z = ndc.z;
  return Vec4(x, y, z, posClip.w);
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

std::array<Vec3, 3> Renderer::fetchNs(const MeshData &mesh,
                                      const TriangleData &tri,
                                      const MaterialData &material) const {

  return {mesh.normals[tri.n0], mesh.normals[tri.n1], mesh.normals[tri.n2]};
}
std::array<Vec3, 3> Renderer::fetchPs(const MeshData &mesh,
                                      const TriangleData &tri,
                                      const MaterialData &material) const {

  return {mesh.positions[tri.i0], mesh.positions[tri.i1],
          mesh.positions[tri.i2]};
}

void Renderer::rasterizeTriangle(const std::array<Vec4, 3> &projected,
                                 const std::array<Vec3, 3> &ps,
                                 const std::array<Vec3, 3> &uvs,
                                 const std::array<Vec3, 3> &ns,
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

        Vec3 worldPos = ps[0] * alpha + ps[1] * beta + ps[2] * gamma;

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
        float remappedU =
            material.uvMin[0] + u * (material.uvMax[0] - material.uvMin[0]);
        float remappedV =
            material.uvMin[1] + v * (material.uvMax[1] - material.uvMin[1]);
        Vec3 interpolatedNormal = (ns[0] * (alpha / projected[0].w) +
                                   ns[1] * (beta / projected[1].w) +
                                   ns[2] * (gamma / projected[2].w)) /
                                  invW;

        interpolatedNormal = interpolatedNormal.normalized();

        Vec4 finalColor = shadeFragment(worldPos, Vec3(remappedU, remappedV, 0),
                                        interpolatedNormal, ctx, material);

        drawPixel(x, y, depth, packColor(finalColor));
      }
    }
  }
}

bool Renderer::isTriangleVisibleInFrustum(const Vec4 &v0, const Vec4 &v1,
                                          const Vec4 &v2) {

  auto inside = [](const Vec4 &v) {
    return v.x >= -v.w && v.x <= v.w && v.y >= -v.w && v.y <= v.w && v.z >= 0 &&
           v.z <= v.w && v.w > 0;
  };

  return inside(v0) || inside(v1) || inside(v2);
}
Vec4 Renderer::shadeFragment(const Vec3 &worldPos, const Vec3 &uv,
                             const Vec3 &normal, const RenderContext &ctx,
                             const MaterialData &material) {

  Vec4 finalColor = material.baseColor;

  if (material.useTexture && material.texture) {
    uint32_t texColor = material.texture->sample(uv.x, uv.y);
    finalColor = unpackColor(texColor);
  }
  if (!ctx.enableLighting) {
    return finalColor;
  }

  Vec3 lightDirView =
      (ctx.viewMatrix * Vec4(lightDir, 0)).toVec3().normalized();
  Vec3 normalView = (ctx.viewMatrix * Vec4(normal, 0)).toVec3().normalized();
  Vec3 viewDir = (ctx.cameraPosition - worldPos).normalized();

  float diff = std::max(material.ambient, normalView.dot(lightDirView * -1));
  Vec3 reflectDir = reflect(lightDirView * -1, normalView);
  float spec =
      std::pow(std::max(0.0f, viewDir.dot(reflectDir)), material.shininess);

  Vec3 rgb = finalColor.xyz() * (diff + material.specular * spec);
  rgb.x = std::clamp(rgb.x, 0.0f, 1.0f);
  rgb.y = std::clamp(rgb.y, 0.0f, 1.0f);
  rgb.z = std::clamp(rgb.z, 0.0f, 1.0f);
  float alpha = finalColor.w;
  return Vec4(rgb, alpha);

  return finalColor;
}

void Renderer::drawTriangle(const MeshData &mesh, const TriangleData &tri,
                            const Mat4 &model, const RenderContext &ctx,
                            const MaterialData &material) {
  Vec4 v0_cam = toCameraSpace(mesh.positions[tri.i0], model, ctx.viewMatrix);
  Vec4 v1_cam = toCameraSpace(mesh.positions[tri.i1], model, ctx.viewMatrix);
  Vec4 v2_cam = toCameraSpace(mesh.positions[tri.i2], model, ctx.viewMatrix);

  if (v0_cam.z > 0 && v1_cam.z > 0 && v2_cam.z > 0)
    return;

  Vec4 v0_clip = projectToClipSpace(v0_cam, ctx.projectionMatrix);
  Vec4 v1_clip = projectToClipSpace(v1_cam, ctx.projectionMatrix);
  Vec4 v2_clip = projectToClipSpace(v2_cam, ctx.projectionMatrix);

  if (!isTriangleVisibleInFrustum(v0_clip, v1_clip, v2_clip))
    return;

  auto uvs = fetchUVs(mesh, tri, material);
  auto ns = fetchNs(mesh, tri, material);
  auto ps = fetchPs(mesh, tri, material);

  if (ctx.is2DPass) {
    Vec4 v0_screen = ndcToScreen(v0_clip, screenWidth, screenHeight);
    Vec4 v1_screen = ndcToScreen(v1_clip, screenWidth, screenHeight);
    Vec4 v2_screen = ndcToScreen(v2_clip, screenWidth, screenHeight);

    std::array<Vec4, 3> vertices = {v0_screen, v1_screen, v2_screen};
    if (!isTriangleVisible(vertices, material, ctx))
      return;
    rasterizeTriangle(vertices, ps, uvs, ns, ctx, material);
    return;
  }

  auto clippedTris = clipTriangleAgainstNearPlane(
      {v0_clip, ns[0], uvs[0], ps[0]}, {v1_clip, ns[1], uvs[1], ps[1]},
      {v2_clip, ns[2], uvs[2], ps[2]});
  for (auto &tri : clippedTris) {

    Vec4 v0_screen = ndcToScreen(tri[0].cposition, screenWidth, screenHeight);
    Vec4 v1_screen = ndcToScreen(tri[1].cposition, screenWidth, screenHeight);
    Vec4 v2_screen = ndcToScreen(tri[2].cposition, screenWidth, screenHeight);

    std::array<Vec4, 3> vertices = {v0_screen, v1_screen, v2_screen};
    if (!isTriangleVisible(vertices, material, ctx))
      return;
    std::array<Vec3, 3> nuvs = {tri[0].uv, tri[1].uv, tri[2].uv};
    std::array<Vec3, 3> nns = {tri[0].normal, tri[1].normal, tri[2].normal};
    std::array<Vec3, 3> nps = {tri[0].position, tri[1].position,
                               tri[2].position};

    rasterizeTriangle(vertices, nps, nuvs, nns, ctx, material);
  }
}

std::vector<std::vector<ClippableVertex>>
Renderer::clipTriangleAgainstNearPlane(const ClippableVertex &v0,
                                       const ClippableVertex &v1,
                                       const ClippableVertex &v2) {
  auto inside = [](const Vec4 &v) { return v.z >= 0.0f && v.z <= v.w; };

  auto intersect = [](const ClippableVertex &a, const ClippableVertex &b) {
    float t = (0.0f - a.cposition.z) / (b.cposition.z - a.cposition.z);
    return a.lerp(b, t);
  };

  std::vector<ClippableVertex> verts = {v0, v1, v2};
  std::vector<ClippableVertex> in, out;
  for (const auto &v : verts)
    (inside(v.cposition) ? in : out).push_back(v);

  if (in.size() == 3)
    return {{{v0, v1, v2}}};
  if (in.size() == 0)
    return {};

  if (in.size() == 1) {
    ClippableVertex i0 = intersect(in[0], out[0]);
    ClippableVertex i1 = intersect(in[0], out[1]);
    return {{{in[0], i0, i1}}};
  }

  if (in.size() == 2) {
    ClippableVertex i0 = intersect(in[0], out[0]);
    ClippableVertex i1 = intersect(in[1], out[0]);
    return {{{in[0], in[1], i1}}, {in[0], i1, i0}};
  }

  return {};
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
  mat.baseColor = sprite.color;
  mat.doubleSided = true;
  mat.uvMin = sprite.uvMin;
  mat.uvMax = sprite.uvMax;

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

void Renderer::drawText(Font *font, const std::string &text, Vec3 pos,
                        float size, Vec4 color) {
  if (!font || !font->sdlFont || text.empty())
    return;

  textDrawQueue.push_back({font, text, pos, size, color});
}

void Renderer::flushTextDraws() {
  for (const auto &cmd : textDrawQueue) {
    SDL_Color sdlColor = {static_cast<Uint8>(cmd.color.x * 255),
                          static_cast<Uint8>(cmd.color.y * 255),
                          static_cast<Uint8>(cmd.color.z * 255),
                          static_cast<Uint8>(cmd.color.w * 255)};

    SDL_Surface *textSurface =
        TTF_RenderUTF8_Blended(cmd.font->sdlFont, cmd.text.c_str(), sdlColor);
    if (!textSurface) {
      std::cerr << "Failed to render text surface: " << TTF_GetError()
                << std::endl;
      continue;
    }

    SDL_Texture *textTexture =
        SDL_CreateTextureFromSurface(sdlRenderer, textSurface);
    SDL_FreeSurface(textSurface);
    if (!textTexture) {
      std::cerr << "Failed to create texture from text surface: "
                << SDL_GetError() << std::endl;
      continue;
    }

    int texW = 0, texH = 0;
    SDL_QueryTexture(textTexture, nullptr, nullptr, &texW, &texH);

    SDL_Rect dstRect;
    dstRect.x = static_cast<int>(cmd.pos.x);
    dstRect.y = static_cast<int>(cmd.pos.y);
    dstRect.w = texW;
    dstRect.h = texH;

    SDL_RenderCopy(sdlRenderer, textTexture, nullptr, &dstRect);
    SDL_DestroyTexture(textTexture);
  }

  textDrawQueue.clear();
}

// void Renderer::drawText(Font* font, const std::string& str, Vec3 pos, float
// size, Vec4 color) {
//     float cursorX = pos.x;
//
//     for (char c : str) {
//         const Glyph& glyph = font->getGlyph(c);
//
//         Vec3 charPos = {cursorX + glyph.offsetX * size, pos.y + glyph.offsetY
//         * size, 0}; Vec3 charSize = {glyph.width * size, glyph.height * size,
//         0};
//
//
//         SpriteData sprite;
//         sprite.texture = font->atlas;
//         sprite.useTexture = true;
//         sprite.color = color;
//         sprite.size = charSize;
//
//         Mat4 model = Mat4::translate(Vec3(charPos.x, charPos.y, 0));
//
//         sprite.uvMin = glyph.uvMin;
//         sprite.uvMax = glyph.uvMax;
//
//         renderSprite(sprite, model);
//
//         cursorX += glyph.advance * size;
//     }
// }

std::array<int, 2> Renderer::getScreenSize() {
  return {screenWidth, screenHeight};
}
} // namespace farixEngine::renderer
