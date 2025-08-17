#include "farixEngine/renderer/opengl/openglRenderer.hpp"
#include "farixEngine/AssetConfig.h"
#include "farixEngine/renderer/opengl/shader.hpp"
#include "farixEngine/renderer/renderData.hpp"
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

  textShaderProgram =
      Shader(std::string(FARIX_ASSET_DIR) + "/shaders/text.vert",
             std::string(FARIX_ASSET_DIR) + "/shaders/text.frag");

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
    for (auto &mesh : pass.gpuMeshCommands) {
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
  GPUMaterialData gpuMaterial;
  gpuMaterial.texture = gpuTexture;
  gpuMaterial.useTexture = material.useTexture;
  gpuMaterial.baseColor = material.baseColor;
  gpuMaterial.ambient = material.ambient;
  gpuMaterial.diffuse = material.diffuse;
  gpuMaterial.doubleSided = material.doubleSided;
  gpuMaterial.shininess = material.shininess;
  gpuMaterial.specular = material.specular;
  gpuMaterial.uuid = material.uuid;
  gpuMaterial.uvMax = material.uvMax;
  gpuMaterial.uvMin = material.uvMin;
  GPUMeshCommand meshCommand{gpuMesh, gpuMaterial, model};
  activePass->gpuMeshCommands.push_back(meshCommand);
}

void OpenGLRenderer::submitSprite(const SpriteData &sprite, const Mat4 &model) {
  std::shared_ptr<MeshData> quadMesh = quadMesh2D();

  Mat4 scaleMat = Mat4::scale(Vec3(sprite.size[0], sprite.size[1], 1.0f));
  Mat4 finalMat = model * scaleMat;

  auto gpuMesh = createOrGetGPUMesh(quadMesh);
  auto gpuTexture = createOrGetGPUTexture(sprite.texture);
  GPUMaterialData mat;
  mat.useTexture = sprite.useTexture;
  mat.texture = gpuTexture;
  mat.baseColor = sprite.color;
  mat.doubleSided = true;
  mat.uvMin = sprite.uvMin;
  mat.uvMax = sprite.uvMax;
  GPUMeshCommand meshCommand{gpuMesh, mat, model};
  activePass->gpuMeshCommands.push_back(meshCommand);
}

void OpenGLRenderer::submitText2D(Font *font, const std::string &str, Vec3 pos,
                                  float size, Vec4 color) {
  UITextDrawCommand cmd{font, str, pos, size, color};
  activePass->textCommands.push_back(cmd);
}

void OpenGLRenderer::submitText(Font *font, const std::string &str, Vec3 pos,
                                float size, Vec4 color, Mat4 model) {
  SDL_Color sdlColor = {(Uint8)(color.x * 255), (Uint8)(color.y * 255),
                        (Uint8)(color.z * 255), (Uint8)(color.w * 255)};

  std::string cacheKey =
      str + "_" + font->id + "_" + std::to_string(sdlColor.r) + "_" +
      std::to_string(sdlColor.g) + "_" + std::to_string(sdlColor.b) + "_" +
      std::to_string(sdlColor.a);

  std::shared_ptr<Texture> textTex;

  if (auto it = gpuTextureCache.find(cacheKey); it != gpuTextureCache.end()) {
    textTex = it->second;
  } else {
    SDL_Surface *srf =
        TTF_RenderUTF8_Blended(font->sdlFont, str.c_str(), sdlColor);
    if (!srf) {
      std::cerr << "TTF_RenderUTF8_Blended failed: " << TTF_GetError() << "\n";
      return;
    }

    SDL_Surface *rgba =
        SDL_ConvertSurfaceFormat(srf, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(srf);
    if (!rgba) {
      std::cerr << "SDL_ConvertSurfaceFormat failed\n";
      return;
    }

    const int w = rgba->w, h = rgba->h;

    unsigned char *tight = (unsigned char *)malloc(w * h * 4);
    if (!tight) {
      SDL_FreeSurface(rgba);
      std::cerr << "malloc failed\n";
      return;
    }
    for (int y = 0; y < h; ++y) {
      memcpy(tight + y * w * 4, (unsigned char *)rgba->pixels + y * rgba->pitch,
             w * 4);
    }
    SDL_FreeSurface(rgba);

    ::farixEngine::Texture tempTex;
    tempTex.texWidth = w;
    tempTex.texHeight = h;
    tempTex.texturePixels = tight;

    textTex = std::make_shared<Texture>(&tempTex, GL_TEXTURE_2D, 1, GL_RGBA,
                                        GL_UNSIGNED_BYTE);

    // free(tight);

    gpuTextureCache.emplace(cacheKey, textTex);
  }

  std::shared_ptr<GPUMesh> quadMesh;
  const std::string meshKey = "textQuad_" + cacheKey;
  if (auto it = gpuMeshCache.find(meshKey); it != gpuMeshCache.end()) {
    quadMesh = it->second;
  } else {
    // const float w = (float)textTex->width;
    // const float h = (float)textTex->height;
    const float aspectRatio = (float)textTex->width / (float)textTex->height;
    const float w = aspectRatio / 2;
    const float h = 0.5f;

    MeshData mesh;
    mesh.vertices.resize(4);
    mesh.indices = {0, 1, 2, 2, 3, 0};

    for (int i = 0; i < 4; ++i)
      mesh.vertices[i].normal = {0.0f, 0.0f, 1.0f};

    mesh.vertices[0].position = {-w, -h, 0.0f};

    mesh.vertices[1].position = {w, -h, 0.0f};

    mesh.vertices[2].position = {w, h, 0.0f};

    mesh.vertices[3].position = {-w, h, 0.0f};

    mesh.vertices[0].uv = {0.0f, 1.0f};
    mesh.vertices[1].uv = {1.0f, 1.0f};
    mesh.vertices[2].uv = {1.0f, 0.0f};
    mesh.vertices[3].uv = {0.0f, 0.0f};

    quadMesh = std::make_shared<GPUMesh>(mesh);
    gpuMeshCache.emplace(meshKey, quadMesh);
  }
  GPUMaterialData mat;
  mat.baseColor = color;
  mat.doubleSided = true;
  mat.texture = textTex;
  mat.useTexture = true;

  GPUMeshCommand meshCommand{quadMesh, mat, model};
  activePass->gpuMeshCommands.push_back(meshCommand);
}

void OpenGLRenderer::renderMesh(const GPUMeshCommand &meshCommand) {

  auto gpuMesh = meshCommand.gpuMesh;

  std::shared_ptr<renderer::Texture> gpuTex = meshCommand.gpuMatData.texture;

  defaultShaderProgram.Activate();

  defaultShaderProgram.setMat4("model", meshCommand.modelMatrix);
  defaultShaderProgram.setMat4("camMatrix", currentContext->projectionMatrix *
                                                currentContext->viewMatrix);

  defaultShaderProgram.setVec4("objectColor", meshCommand.gpuMatData.baseColor);
  defaultShaderProgram.setBool("useTexture", meshCommand.gpuMatData.useTexture);

  defaultShaderProgram.setBool("enableLight", currentContext->enableLighting);

  defaultShaderProgram.setVec4("lightColor", currentContext->lightColor);
  defaultShaderProgram.setVec3("lightPos", currentContext->lightPos);
  defaultShaderProgram.setVec3("camPos", currentContext->cameraPosition);
  defaultShaderProgram.setFloat("matAmbient", meshCommand.gpuMatData.ambient);
  defaultShaderProgram.setFloat("matDiffuse", meshCommand.gpuMatData.diffuse);
  defaultShaderProgram.setFloat("matSpecular", meshCommand.gpuMatData.specular);
  defaultShaderProgram.setFloat("matShininess",
                                meshCommand.gpuMatData.shininess);

  if (gpuTex) {
    gpuTex->Bind();
    gpuTex->texUnit(defaultShaderProgram, "tex0", gpuTex->unit);
  }

  gpuMesh->vao.Bind();

  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gpuMesh->indexCount),
                 GL_UNSIGNED_INT, 0);

  gpuMesh->vao.Unbind();
  if (gpuTex)
    gpuTex->unBind();
}

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
  defaultShaderProgram.setFloat("matDiffuse", meshCommand.matData.diffuse);
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

// !!! TO BE EDITED !!!

void OpenGLRenderer::renderText(const UITextDrawCommand &cmd) {

  SDL_Color sdlColor = {(Uint8)(cmd.color.x * 255), (Uint8)(cmd.color.y * 255),
                        (Uint8)(cmd.color.z * 255), (Uint8)(cmd.color.w * 255)};

  std::string cacheKey =
      cmd.text + "_" + cmd.font->id + "_" + std::to_string(sdlColor.r) + "_" +
      std::to_string(sdlColor.g) + "_" + std::to_string(sdlColor.b) + "_" +
      std::to_string(sdlColor.a);

  std::shared_ptr<Texture> textTex;
  if (auto it = gpuTextureCache.find(cacheKey); it != gpuTextureCache.end()) {
    textTex = it->second;
  } else {
    SDL_Surface *srf =
        TTF_RenderUTF8_Blended(cmd.font->sdlFont, cmd.text.c_str(), sdlColor);
    if (!srf) {
      std::cerr << "TTF_RenderUTF8_Blended failed: " << TTF_GetError() << "\n";
      return;
    }

    SDL_Surface *rgba =
        SDL_ConvertSurfaceFormat(srf, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(srf);
    if (!rgba) {
      std::cerr << "SDL_ConvertSurfaceFormat failed\n";
      return;
    }

    const int w = rgba->w, h = rgba->h;

    unsigned char *tight = (unsigned char *)malloc(w * h * 4);
    if (!tight) {
      SDL_FreeSurface(rgba);
      std::cerr << "malloc failed\n";
      return;
    }
    for (int y = 0; y < h; ++y) {
      memcpy(tight + y * w * 4, (unsigned char *)rgba->pixels + y * rgba->pitch,
             w * 4);
    }
    SDL_FreeSurface(rgba);

    ::farixEngine::Texture tempTex;
    tempTex.texWidth = w;
    tempTex.texHeight = h;
    tempTex.texturePixels = tight;

    textTex = std::make_shared<Texture>(&tempTex, GL_TEXTURE_2D, 1, GL_RGBA,
                                        GL_UNSIGNED_BYTE);

    // free(tight);

    gpuTextureCache.emplace(cacheKey, textTex);
  }

  std::shared_ptr<GPUMesh> quadMesh;
  const std::string meshKey = "textQuad_" + cacheKey;
  if (auto it = gpuMeshCache.find(meshKey); it != gpuMeshCache.end()) {
    quadMesh = it->second;
  } else {
    const float w = (float)textTex->width / 2;
    const float h = (float)textTex->height / 2;

    MeshData mesh;
    mesh.vertices.resize(4);
    mesh.indices = {0, 1, 2, 2, 3, 0};

    for (int i = 0; i < 4; ++i)
      mesh.vertices[i].normal = {0.0f, 0.0f, 1.0f};

    mesh.vertices[0].position = {cmd.pos.x - w, cmd.pos.y - h, 0.0f};
    mesh.vertices[0].uv = {0.0f, 0.0f};
    mesh.vertices[1].position = {cmd.pos.x + w, cmd.pos.y - h, 0.0f};
    mesh.vertices[1].uv = {1.0f, 0.0f};
    mesh.vertices[2].position = {cmd.pos.x + w, cmd.pos.y + h, 0.0f};
    mesh.vertices[2].uv = {1.0f, 1.0f};
    mesh.vertices[3].position = {cmd.pos.x - w, cmd.pos.y + h, 0.0f};
    mesh.vertices[3].uv = {0.0f, 1.0f};

    quadMesh = std::make_shared<GPUMesh>(mesh);
    gpuMeshCache.emplace(meshKey, quadMesh);
  }

  textShaderProgram.Activate();
  textShaderProgram.setVec2("uScreenSize", Vec2(screenWidth, screenHeight));
  textShaderProgram.setVec4("textColor", cmd.color);

  textTex->Bind();
  textShaderProgram.setInt("textTexture", 1);

  quadMesh->vao.Bind();
  glDrawElements(GL_TRIANGLES, (GLsizei)quadMesh->indexCount, GL_UNSIGNED_INT,
                 0);
  quadMesh->vao.Unbind();

  textTex->unBind();
}
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
