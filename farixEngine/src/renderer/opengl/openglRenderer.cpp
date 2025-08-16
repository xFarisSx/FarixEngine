#include "farixEngine/renderer/opengl/openglRenderer.hpp"
#include "farixEngine/AssetConfig.h"
#include "farixEngine/renderer/opengl/shader.hpp"
#include <array>
namespace farixEngine::renderer {

OpenGLRenderer::OpenGLRenderer(int width, int height, const char *title)
    : IRenderer(width, height, title) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!window) {
    std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
    return;
  }

  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (!glContext) {
    std::cerr << "Failed to create OpenGL context: " << SDL_GetError()
              << std::endl;
    return;
  }

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return;
  }

  int swidth, sheight;
  SDL_GL_GetDrawableSize(window, &swidth, &sheight);
  glViewport(0, 0, swidth, sheight);

  if (TTF_Init() == -1) {
    std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError()
              << std::endl;
    exit(1);
  }
  //
  defaultShaderProgram =
      Shader(std::string(FARIX_ASSET_DIR) + "/shaders/default.vert",
             std::string(FARIX_ASSET_DIR) + "/shaders/default.frag");
}

OpenGLRenderer::~OpenGLRenderer() { SDL_DestroyWindow(window); }

void OpenGLRenderer::beginFrame(const RenderContext &context) { clear(); }
void OpenGLRenderer::setContext(const RenderContext &context) {}
void OpenGLRenderer::beginUIPass(int screenW, int screenH) {}
RenderContext OpenGLRenderer::makeUIContext(int screenW, int screenH) {}
void OpenGLRenderer::endFrame() { present(); }

void OpenGLRenderer::clear(uint32_t color) {

  glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void OpenGLRenderer::present() { SDL_GL_SwapWindow(window); }

void OpenGLRenderer::renderMesh(const MeshData &mesh, const Mat4 &model,
                                const MaterialData &material) {}

void OpenGLRenderer::renderSprite(const SpriteData &sprite, const Mat4 &model) {
}

void OpenGLRenderer::drawText(Font *font, const std::string &str, Vec3 pos,
                              float size, Vec4 color) {}

std::array<int, 2> OpenGLRenderer::getScreenSize() {
  return {screenWidth, screenHeight};
}

} // namespace farixEngine::renderer
