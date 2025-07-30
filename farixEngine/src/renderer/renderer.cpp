
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

namespace farixEngine {

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

void Renderer::clear(uint32_t color) {
  if (!framebuffer) {
    std::cerr << "Error: framebuffer is null!" << std::endl;
    return;
  }
  if (screenWidth == 0 || screenHeight == 0) {
    std::cerr << "Error: screenWidth or screenHeight is zero!" << std::endl;
    return;
  }

  Uint8 skyR = 135;
  Uint8 skyG = 206;
  Uint8 skyB = 235;

  Uint32 clearColor = (skyR << 16) | (skyG << 8) | skyB;
  std::fill(framebuffer, framebuffer + screenWidth * screenHeight, clearColor);
  std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::max());
}

void Renderer::present() {
  SDL_UpdateTexture(sdlTexture, nullptr, framebuffer,
                    screenWidth * sizeof(uint32_t));
  SDL_RenderClear(sdlRenderer);
  SDL_RenderCopy(sdlRenderer, sdlTexture, nullptr, nullptr);
  SDL_RenderPresent(sdlRenderer);
}

Vec4 Renderer::project(const Vec4 &point, const Mat4 &globalMat,
                       const Mat4 &viewM, const Mat4 &perspM) const {
  Vec4 projected4 = perspM * viewM * globalMat * point;

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

auto isValid = [](const Vec4 &v) {
  return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
};

void Renderer::drawTriangle(const Mesh *mesh, const Triangle &tri,
                            const std::vector<Vec3> &vertices,
                            const Mat4 &globalMat,
                            const TransformComponent &cameraTransform,
                            const CameraComponent &camera,
                            const MaterialComponent &material) {
  Vec3 v0 = vertices[tri.i0];
  Vec3 v1 = vertices[tri.i1];
  Vec3 v2 = vertices[tri.i2];
  Vec4 v04 = Vec4(v0.x, v0.y, v0.z, 1);
  Vec4 v14 = Vec4(v1.x, v1.y, v1.z, 1);
  Vec4 v24 = Vec4(v2.x, v2.y, v2.z, 1);

  Vec3 forward, right, up;
  math::updateCameraBasis(cameraTransform.rotation, forward, right, up);

  Mat4 viewM = Mat4::lookAt(cameraTransform.position,
                            cameraTransform.position + forward, Vec3(0, 1, 0));

  Mat4 projection;
  if (camera.mode == CameraProjectionMode::Perspective) {
    projection = Mat4::perspective(camera.fov, camera.aspectRatio,
                                   camera.nearPlane, camera.farPlane);
  } else {

    projection =
        Mat4::ortho(camera.orthoLeft, camera.orthoRight, camera.orthoBottom,
                    camera.orthoTop, camera.orthoNear, camera.orthoFar);
  }

  Vec3 normal = (v1 - v0).cross(v2 - v0);

  Vec3 normalWorld = (globalMat * Vec4(normal, 0.0f)).toVec3().normalized();

  if (!material.doubleSided && normalWorld.dot(forward) > 0.2f) {
    return;
  }

  Vec4 p0 = project(v04, globalMat, viewM, projection);
  Vec4 p1 = project(v14, globalMat, viewM, projection);
  Vec4 p2 = project(v24, globalMat, viewM, projection);

  if (p0.z < 0.0f || p0.z > 1.0f || p0.x < 0 || p0.x >= screenWidth ||
      p0.y < 0 || p0.y >= screenHeight)
    return;
  if (p1.z < 0.0f || p1.z > 1.0f || p1.x < 0 || p1.x >= screenWidth ||
      p1.y < 0 || p1.y >= screenHeight)
    return;
  if (p2.z < 0.0f || p2.z > 1.0f || p2.x < 0 || p2.x >= screenWidth ||
      p2.y < 0 || p2.y >= screenHeight)
    return;

  if (!isValid(p0) || !isValid(p1) || !isValid(p2)) {
    std::cout << "Skipping triangle due to invalid projection values\n";
    return;
  }

  Vec3 uv0;
  Vec3 uv1;
  Vec3 uv2;
  if (material.useTexture) {
    uv0 = mesh->textureMap[tri.uv0];
    uv1 = mesh->textureMap[tri.uv1];
    uv2 = mesh->textureMap[tri.uv2];
  }
  float minX = std::min({p0.x, p1.x, p2.x});
  float maxX = std::max({p0.x, p1.x, p2.x});
  float minY = std::min({p0.y, p1.y, p2.y});
  float maxY = std::max({p0.y, p1.y, p2.y});

  int minXInt = std::max(0, static_cast<int>(std::floor(minX)));
  int maxXInt = std::min(screenWidth - 1, static_cast<int>(std::ceil(maxX)));
  int minYInt = std::max(0, static_cast<int>(std::floor(minY)));
  int maxYInt = std::min(screenHeight - 1, static_cast<int>(std::ceil(maxY)));

  float area = edgeFunction(p0, p1, p2);
  if (std::abs(area) < 1e-6f) {
    return;
  }

  Vec3 lightDirViewSpace = (viewM * Vec4(lightDir, 0.0f)).toVec3().normalized();
  for (int y = minYInt; y <= maxYInt; ++y) {
    for (int x = minXInt; x <= maxXInt; ++x) {
      Vec3 c(x, y, 0);

      float w0 = edgeFunction(p1, p2, c);
      float w1 = edgeFunction(p2, p0, c);
      float w2 = edgeFunction(p0, p1, c);

      if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {

        float alpha = w0 / area;
        float beta = w1 / area;
        float gamma = w2 / area;

        float depth = alpha * p0.z + beta * p1.z + gamma * p2.z;
        if (!std::isfinite(depth) || depth < 0 || depth > 1) {
          continue;
        }

        Vec3 worldPos = v0 * alpha + v1 * beta + v2 * gamma;
        float invW0 = 1.0f / p0.w;
        float invW1 = 1.0f / p1.w;
        float invW2 = 1.0f / p2.w;
        float invW = alpha * invW0 + beta * invW1 + gamma * invW2;

        float uOverW = alpha * (uv0.x * invW0) + beta * (uv1.x * invW1) +
                       gamma * (uv2.x * invW2);
        float vOverW = alpha * (uv0.y * invW0) + beta * (uv1.y * invW1) +
                       gamma * (uv2.y * invW2);

        float u = uOverW / invW;
        float v = vOverW / invW;

        Vec3 finalColor = material.baseColor;

        if (material.useTexture && material.texture) {
          // float u = alpha * uv0.x + beta * uv1.x + gamma * uv2.x;
          // float v = alpha * uv0.y + beta * uv1.y + gamma * uv2.y;

          Uint32 texColor = material.texture->sample(u, v);

          Uint8 r = (texColor >> 16) & 0xFF;
          Uint8 g = (texColor >> 8) & 0xFF;
          Uint8 b = texColor & 0xFF;

          finalColor = Vec3(r / 255.0f, g / 255.0f, b / 255.0f);
        }

        Vec3 normalViewSpace =
            (viewM * Vec4(normalWorld, 0.0f)).toVec3().normalized();

        // Ambient + diffuse lighting
        float diffIntensity = std::max(
            material.ambient, normalViewSpace.dot(lightDirViewSpace * -1));

        Vec3 viewDir = (cameraTransform.position - worldPos).normalized();

        // Reflection direction for specular
        Vec3 reflectDir = reflect(lightDirViewSpace * -1, normalViewSpace);

        // Specular intensity
        float spec =
            pow(std::max(0.0f, viewDir.dot(reflectDir)), material.shininess);

        // Total light intensity combining ambient, diffuse, specular
        float totalLight = diffIntensity + material.specular * spec;

        Vec3 litColor = finalColor * totalLight;

        Uint8 r =
            static_cast<Uint8>(std::clamp(litColor.x * 255.0f, 0.0f, 255.0f));
        Uint8 g =
            static_cast<Uint8>(std::clamp(litColor.y * 255.0f, 0.0f, 255.0f));
        Uint8 b =
            static_cast<Uint8>(std::clamp(litColor.z * 255.0f, 0.0f, 255.0f));

        Uint32 color = (r << 16) | (g << 8) | b;

        drawPixel(x, y, depth, color);
      }
    }
  }
}

void Renderer::renderMesh(const Mesh *mesh, const Mat4 &globalMat,
                          const TransformComponent &cameraTransform,
                          const CameraComponent &camera,
                          const MaterialComponent &material) {
  if (!mesh) {

    return;
  }

  SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 255);
  for (const Triangle &tri : mesh->triangles) {
    drawTriangle(mesh, tri, mesh->vertices, globalMat, cameraTransform, camera,
                 material);
  }
}
//
// void Renderer::renderWorld(Mesh *mesh,
//                            const TransformComponent &entityTransform,
//                            const TransformComponent &cameraTransform,
//                            const CameraComponent &camera) {
//   renderMesh(mesh, entityTransform, cameraTransform, camera);
// }
//
} // namespace farixEngine
