#pragma once

#include "farixEngine/thirdparty/glad/glad.h"

namespace farixEngine::renderer {

class EBO {
public:
  GLuint ID;
  EBO(GLuint *indices, GLsizeiptr size);
  EBO() = default;

  void Bind();
  void Unbind();
  void Delete();
};
} // namespace farixEngine::renderer
