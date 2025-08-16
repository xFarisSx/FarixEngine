#pragma once
#include <glad/glad.h>

namespace farixEngine::renderer {

class VBO {
public:
  GLuint ID;
  VBO(GLfloat *vertices, GLsizeiptr size);

  void Bind();
  void Unbind();
  void Delete();
};
} // namespace farixEngine::renderer
