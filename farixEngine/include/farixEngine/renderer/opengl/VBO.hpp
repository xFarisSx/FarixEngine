#pragma once
#include "farixEngine/thirdparty/glad/glad.h"

namespace farixEngine::renderer {

class VBO {
public:
  GLuint ID;
  VBO(GLfloat *vertices, GLsizeiptr size);
  VBO() = default;

  void Bind();
  void Unbind();
  void Delete();
};
} // namespace farixEngine::renderer
