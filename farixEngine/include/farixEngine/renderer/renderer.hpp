
#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <limits>
#include <vector>

#include "farixEngine/assets/mesh.hpp"
#include "farixEngine/components/components.hpp"
#include "farixEngine/input/controller.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/math/vec4.hpp"
 
namespace farixEngine {

class Renderer {
public:
  Renderer(int width, int height, const char *title);
  ~Renderer();

  void clear(uint32_t color = 0xFF000000);
  void present();

  Vec3 project(const Vec4 &point, const Mat4 &globalMat,
                       const Mat4 &viewM, const Mat4 &perspM) const ;

  void drawPixel(int x, int y, float z, uint32_t color);

  float edgeFunction(const Vec3 &a, const Vec3 &b, const Vec3 &c) const;

  void drawTriangle(const Mesh *mesh, const Triangle &tri,
                    const std::vector<Vec3> &vertices,
                    const  Mat4 & globalMat,
                    const TransformComponent &cameraTransform,
                    const CameraComponent &camera, const MaterialComponent& material);

  void renderMesh(const Mesh *mesh, const Mat4 & globalMat,
                  const TransformComponent &cameraTransform,
                  const CameraComponent &camera, const MaterialComponent& material);

  void renderWorld(Mesh *mesh, const TransformComponent &entityTransform,
                   const TransformComponent &cameraTransform,
                   const CameraComponent &camera);
  Vec3 reflect(const Vec3& L, const Vec3& N) ;
  private:
  SDL_Window *window = nullptr;
  SDL_Renderer *sdlRenderer = nullptr; 
  SDL_Texture *sdlTexture = nullptr;

  int screenWidth = 0;
  int screenHeight = 0;

  uint32_t *framebuffer = nullptr;
  std::vector<float> zBuffer;

  Vec3 lightDir = Vec3(0, 0, 1);
};

} // namespace farixEngine
