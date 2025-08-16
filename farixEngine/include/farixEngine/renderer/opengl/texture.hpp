#include "farixEngine/assets/texture.hpp"
#include "farixEngine/renderer/opengl/shader.hpp"
#include "farixEngine/thirdparty/glad/glad.h"
#include "farixEngine/thirdparty/stb_image.h"

namespace farixEngine::renderer {

class Texture {
public:
  GLuint ID;
  GLenum type;
  GLuint unit;
  Texture(::farixEngine::Texture *texture, GLenum texType, GLuint slot,
          GLenum format, GLenum pixelType);

  void texUnit(Shader &shader, const char *uniform, GLuint unit);
  void Bind();
  void unBind();
  void Delete();
};

} // namespace farixEngine::renderer
