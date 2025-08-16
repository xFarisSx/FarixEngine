#pragma once

#include "farixEngine/renderer/opengl/VBO.hpp"
#include "farixEngine/thirdparty/glad/glad.h"

namespace farixEngine::renderer {

class VAO {
public:
  GLuint ID;
  VAO();
  void LinkAttrib(VBO &VBO, GLuint layout, GLuint numComponents, GLenum type,
                  GLsizeiptr stride, void *offset);
  void Bind();
  void Unbind();
  void Delete();
};
} // namespace farixEngine::renderer
