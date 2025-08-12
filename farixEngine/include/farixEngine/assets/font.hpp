#pragma once
#include "farixEngine/utils/uuid.hpp"
#include "farixEngine/assets/assetManager.hpp"
#include <SDL_ttf.h>
#include <iostream>
#include <memory>
namespace farixEngine {
struct Font : public Asset {
  TTF_Font* sdlFont = nullptr;
  std::string path="";
  std::string id ="";
  int ptsize;
  

  ~Font() {
    if (sdlFont)
      TTF_CloseFont(sdlFont);
  }

    static std::shared_ptr<Font> load(const std::string&filename, int ptsize){
    return loadFont(filename,ptsize);
  }
  static std::shared_ptr<Font> loadFont(const std::string &path, int ptsize, std::string eid = "") {
    auto font = std::make_shared<Font>();

    font->sdlFont = (TTF_OpenFont(path.c_str(), ptsize));
    if (!font->sdlFont) {
      std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
      return nullptr;
    }
    font->id = eid.empty() ? utils::generateUUID() : eid;
    font->path = path;
    font->ptsize = ptsize;
    return font;
  }
};
} // namespace farixEngine
