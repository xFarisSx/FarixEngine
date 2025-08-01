#pragma once
#include <SDL_ttf.h>
#include <iostream>
#include <memory>
namespace farixEngine {
struct Font {
  TTF_Font* sdlFont = nullptr;

  ~Font() {
    if (sdlFont)
      TTF_CloseFont(sdlFont);
  }

  static std::shared_ptr<Font> loadFont(const std::string &path, int ptsize) {
    auto font = std::make_shared<Font>();
    font->sdlFont = (TTF_OpenFont(path.c_str(), ptsize));
    if (!font->sdlFont) {
      std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
      return nullptr;
    }
    return font;
  }
};
} // namespace farixEngine
