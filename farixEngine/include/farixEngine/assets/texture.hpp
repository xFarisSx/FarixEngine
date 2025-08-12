#pragma once
#include "farixEngine/assets/assetManager.hpp"
#include <SDL2/SDL.h>
#include <string>

#include <memory>

namespace farixEngine {

struct Texture:public Asset {
  std::string id;
  std::string path;

  SDL_Surface *textureSurface = nullptr;
  Uint32 *texturePixels = nullptr;
  int texWidth = 0;
  int texHeight = 0;


  ~Texture() {
    if (textureSurface)
      SDL_FreeSurface(textureSurface);
  }
    static std::shared_ptr<Texture> load(const std::string&filename);
  static std::shared_ptr<Texture> loadFromBmp(const std::string &filename,  std::string eid="");
  Uint32 sample(float u, float v) const;
};
} // namespace farixEngine
