#include "farixEngine/renderer/opengl/openglRenderer.hpp"
#include "farixEngine/AssetConfig.h"
#include "farixEngine/renderer/opengl/shader.hpp"
#include "farixEngine/renderer/renderer.hpp"

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

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
}

OpenGLRenderer::~OpenGLRenderer() { SDL_DestroyWindow(window); }

void OpenGLRenderer::beginFrame() {
  clear();
  passes.clear();
  activePass = nullptr;
  currentContext = nullptr;
}
void OpenGLRenderer::beginPass(RenderContext &renderContext) {
  RenderPass rp;
  rp.context = renderContext;
  passes.push_back(rp);
  activePass = &passes[passes.size() - 1];
  currentContext = &renderContext;
}
void OpenGLRenderer::setContext(RenderContext &context) {
  currentContext = &context;
}
void OpenGLRenderer::endPass() {
  activePass = nullptr;
  currentContext = nullptr;
}

void OpenGLRenderer::endFrame() {

  for (auto &pass : passes) {
    setContext(pass.context);
    if (pass.context.enableZBuffer) {
      glEnable(GL_DEPTH_TEST);

    } else {
      glDisable(GL_DEPTH_TEST);
    }
    if (pass.context.is2DPass || pass.context.isOrthographic) {
      glDisable(GL_CULL_FACE);
    } else {
      glEnable(GL_CULL_FACE);
    }

    for (auto &mesh : pass.meshCommands) {
      renderMesh(mesh);
    }
    for (auto &text : pass.textCommands) {
      renderText(text);
    }
  }
  present();
}

void OpenGLRenderer::clear(uint32_t color) {
  Vec4 unpacked = unpackColor(color);

  glClearColor(unpacked.x, unpacked.y, unpacked.z, unpacked.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void OpenGLRenderer::present() { SDL_GL_SwapWindow(window); }

void OpenGLRenderer::submitMesh(const std::shared_ptr<MeshData> mesh,
                                const Mat4 &model,
                                const MaterialData &material) {
  auto gpuMesh = createOrGetGPUMesh(mesh);
  auto gpuTexture = createOrGetGPUTexture(material.texture);
  MeshCommand meshCommand{mesh, material, model};
  activePass->meshCommands.push_back(meshCommand);
}

void OpenGLRenderer::submitSprite(const SpriteData &sprite, const Mat4 &model) {
  std::shared_ptr<MeshData> quadMesh = quadMesh2D();
  MaterialData mat;
  mat.useTexture = sprite.useTexture;
  mat.texture = sprite.texture;
  mat.baseColor = sprite.color;
  mat.doubleSided = true;
  mat.uvMin = sprite.uvMin;
  mat.uvMax = sprite.uvMax;

  Mat4 scaleMat = Mat4::scale(Vec3(sprite.size[0], sprite.size[1], 1.0f));
  Mat4 finalMat = model * scaleMat;

  auto gpuMesh = createOrGetGPUMesh(quadMesh);
  auto gpuTexture = createOrGetGPUTexture(mat.texture);

  MeshCommand meshCommand{quadMesh, mat, finalMat};
  activePass->meshCommands.push_back(meshCommand);
}

void OpenGLRenderer::submitText(Font *font, const std::string &str, Vec3 pos,
                                float size, Vec4 color) {}

void OpenGLRenderer::renderMesh(const MeshCommand &meshCommand) {
  auto gpuMeshIt = gpuMeshCache.find(meshCommand.meshData->uuid);
  if (gpuMeshIt == gpuMeshCache.end())
    return;
  auto gpuMesh = gpuMeshIt->second;

  std::shared_ptr<Texture> gpuTex = nullptr;
  if (meshCommand.matData.useTexture && meshCommand.matData.texture) {
    auto texIt = gpuTextureCache.find(meshCommand.matData.texture->id);
    if (texIt != gpuTextureCache.end())
      gpuTex = texIt->second;
  }

  defaultShaderProgram.Activate();

  defaultShaderProgram.setMat4("model", meshCommand.modelMatrix);
  defaultShaderProgram.setMat4("camMatrix", currentContext->projectionMatrix *
                                                currentContext->viewMatrix);

  defaultShaderProgram.setVec4("objectColor", meshCommand.matData.baseColor);
  defaultShaderProgram.setBool("useTexture", meshCommand.matData.useTexture);

  defaultShaderProgram.setBool("enableLight", currentContext->enableLighting);

  defaultShaderProgram.setVec4("lightColor", currentContext->lightColor);
  defaultShaderProgram.setVec3("lightPos", currentContext->lightPos);
  defaultShaderProgram.setVec3("camPos", currentContext->cameraPosition);
  defaultShaderProgram.setFloat("matAmbient", meshCommand.matData.ambient);
  defaultShaderProgram.setFloat("matSpecular", meshCommand.matData.specular);
  defaultShaderProgram.setFloat("matShininess", meshCommand.matData.shininess);

  if (gpuTex) {
    gpuTex->Bind();
    gpuTex->texUnit(defaultShaderProgram, "tex0", 0);
  }

  gpuMesh->vao.Bind();

  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gpuMesh->indexCount),
                 GL_UNSIGNED_INT, 0);

  gpuMesh->vao.Unbind();
  if (gpuTex)
    gpuTex->unBind();
}
void OpenGLRenderer::renderText(const UITextDrawCommand &textCommand) {}

std::array<int, 2> OpenGLRenderer::getScreenSize() {
  return {screenWidth, screenHeight};
}

std::shared_ptr<GPUMesh>
OpenGLRenderer::createOrGetGPUMesh(const std::shared_ptr<MeshData> &mesh) {
  auto it = gpuMeshCache.find(mesh->uuid);
  if (it != gpuMeshCache.end())
    return it->second;

  auto gpuMesh = std::make_shared<GPUMesh>(*mesh);
  gpuMeshCache[mesh->uuid] = gpuMesh;
  return gpuMesh;
}

std::shared_ptr<Texture>
OpenGLRenderer::createOrGetGPUTexture(::farixEngine::Texture *tex) {
  if (!tex)
    return nullptr;
  auto it = gpuTextureCache.find(tex->id);
  if (it != gpuTextureCache.end())
    return it->second;

  auto gpuTex = std::make_shared<Texture>(tex, GL_TEXTURE_2D, 0, GL_RGBA,
                                          GL_UNSIGNED_BYTE);
  gpuTextureCache[tex->id] = gpuTex;
  return gpuTex;
}

} // namespace farixEngine::renderer
