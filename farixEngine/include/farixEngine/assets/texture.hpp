#pragma once

#include <SDL2/SDL.h>
#include <string>

#include <memory>

namespace farixEngine {

struct Texture {
  std::string path;

  SDL_Surface *textureSurface = nullptr;
  Uint32 *texturePixels = nullptr;
  int texWidth = 0;
  int texHeight = 0;

  ~Texture() {
    if (textureSurface)
      SDL_FreeSurface(textureSurface);
  }

  static std::shared_ptr<Texture> loadFromBmp(const std::string &filename);
  Uint32 sample(float u, float v) const;
};
} // namespace farixEngine
