#pragma once
#include "farixEngine/assets/assetManager.hpp"
#include "farixEngine/thirdparty/stb_image.h"
#include <SDL2/SDL.h>
#include <string>

#include <memory>

namespace farixEngine {

struct Texture : public Asset {
  std::string id;
  std::string path;

  SDL_Surface *textureSurface = nullptr;
  unsigned char *texturePixels = nullptr;
  int texWidth = 0;
  int texHeight = 0;
  int numColCh;

  ~Texture() {
    if (textureSurface)
      SDL_FreeSurface(textureSurface);
    stbi_image_free(texturePixels);
  }
  static std::shared_ptr<Texture> load(const std::string &filename,
                                       std::string eid = "");
  // static std::shared_ptr<Texture> loadFromBmp(const std::string &filename,
  // std::string eid="");
  Uint32 sample(float u, float v) const;
};
} // namespace farixEngine
