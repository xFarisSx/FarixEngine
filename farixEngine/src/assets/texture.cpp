#include "farixEngine/assets/texture.hpp"
#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>

namespace farixEngine{
  
  std::shared_ptr<Texture> Texture::loadFromBmp(const std::string &filename){
    auto texture = std::make_shared<Texture>();
  
    texture->path = filename;

    SDL_Surface *loadedSurface = SDL_LoadBMP(filename.c_str());
    if (!loadedSurface) {
      std::cerr << "Failed to load BMP: " << SDL_GetError() << std::endl;
      exit(1);
    }

    SDL_Surface *formattedSurface =
        SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(loadedSurface);

    if (!formattedSurface) {
      std::cerr << "Failed to convert surface: " << SDL_GetError() << std::endl;
      exit(1);
    }


    texture->textureSurface = formattedSurface;
    texture->texturePixels = static_cast<Uint32 *>(formattedSurface->pixels);
    texture->texWidth = formattedSurface->w;
    texture->texHeight = formattedSurface->h;
  
    return texture;

  }

  Uint32 Texture::sample(float u, float v) const {
    if (!texturePixels) return 0xFFFFFFFF;

    u = std::clamp(u, 0.0f, 1.0f);
    v = std::clamp(v, 0.0f, 1.0f);

    int x = static_cast<int>(u * texWidth);
    int y = static_cast<int>((1.0f - v) * texHeight); // Flip vertically

    x = std::clamp(x, 0, texWidth - 1);
    y = std::clamp(y, 0, texHeight - 1);

    return texturePixels[y * texWidth + x];
}


}
