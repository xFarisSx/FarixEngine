#include "farixEngine/renderer/opengl/VAO.hpp"

namespace farixEngine::renderer {

VAO::VAO() { glGenVertexArrays(1, &ID); }

void VAO::LinkAttrib(VBO &VBO, GLuint layout, GLuint numComponents, GLenum type,
                     GLsizeiptr stride, void *offset) {
  VBO.Bind();
  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  glEnableVertexAttribArray(layout);
  VBO.Unbind();
}

void VAO::Bind() { glBindVertexArray(ID); }

void VAO::Unbind() { glBindVertexArray(0); }
void VAO::Delete() { glDeleteVertexArrays(1, &ID); }

} // namespace farixEngine::renderer
