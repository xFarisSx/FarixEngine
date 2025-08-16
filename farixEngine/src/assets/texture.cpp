#include "farixEngine/assets/texture.hpp"
#include "farixEngine/thirdparty/stb_image.h"
#include "farixEngine/utils/uuid.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

namespace farixEngine {
std::shared_ptr<Texture> Texture::load(const std::string &filename,
                                       std::string eid) {

  auto texture = std::make_shared<Texture>();

  texture->path = filename;
  texture->id = eid.empty() ? utils::generateUUID() : eid;

  int widthImg, heightImg, numColCh;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *bytes = stbi_load(filename.c_str(), &widthImg, &heightImg,
                                   &numColCh, STBI_rgb_alpha);
  if (!bytes) {
    std::cerr << "Failed to load image: " << stbi_failure_reason() << "\n";
  }

  texture->texturePixels = bytes;
  texture->texWidth = widthImg;
  texture->texHeight = heightImg;
  texture->numColCh = numColCh;

  return texture;
}

// std::shared_ptr<Texture> Texture::loadFromBmp(const std::string &filename,
//                                               std::string eid) {
//   auto texture = std::make_shared<Texture>();
//
//   texture->path = filename;
//   texture->id = eid.empty() ? utils::generateUUID() : eid;
//
//   SDL_Surface *loadedSurface = SDL_LoadBMP(filename.c_str());
//   if (!loadedSurface) {
//     std::cerr << "Failed to load BMP: " << SDL_GetError() << std::endl;
//     exit(1);
//   }
//
//   SDL_Surface *formattedSurface =
//       SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_ARGB8888, 0);
//   SDL_FreeSurface(loadedSurface);
//
//   if (!formattedSurface) {
//     std::cerr << "Failed to convert surface: " << SDL_GetError() <<
//     std::endl; exit(1);
//   }
//
//   texture->textureSurface = formattedSurface;
//   texture->texturePixels = static_cast<unsigned
//   char*>(formattedSurface->pixels); texture->texWidth = formattedSurface->w;
//   texture->texHeight = formattedSurface->h;
//
//   return texture;
// }

Uint32 Texture::sample(float u, float v) const {
  if (!texturePixels)
    return 0xFFFFFFFF;

  u = std::clamp(u, 0.0f, 1.0f);
  v = std::clamp(v, 0.0f, 1.0f);

  int x = static_cast<int>(u * texWidth);
  int y = static_cast<int>((v) * texHeight);

  x = std::clamp(x, 0, texWidth - 1);
  y = std::clamp(y, 0, texHeight - 1);

  unsigned char *pixel = texturePixels + (y * texWidth + x) * 4;
  Uint32 r = pixel[0];
  Uint32 g = pixel[1];
  Uint32 b = pixel[2];
  Uint32 a = pixel[3];

  return (a << 24) | (r << 16) | (g << 8) | b;
}

} // namespace farixEngine
