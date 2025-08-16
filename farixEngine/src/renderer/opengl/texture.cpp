#include "farixEngine/renderer/opengl/texture.hpp"
namespace farixEngine::renderer {

Texture::Texture(::farixEngine::Texture *texture, GLenum texType, GLuint slot,
                 GLenum format, GLenum pixelType) {

  type = texType;
  unit = slot;

  glGenTextures(1, &ID);
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(texType, ID);

  glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(texType, 0, GL_RGBA, texture->texWidth, texture->texHeight, 0,
               format, pixelType, texture->texturePixels);

  glGenerateMipmap(texType);

  stbi_image_free(texture->texturePixels);

  glBindTexture(texType, 0);
}

void Texture::texUnit(Shader &shader, const char *uniform, GLuint unit) {
  GLuint tex0Uni = glGetUniformLocation(shader.ID, uniform);
  shader.Activate();
  glUniform1i(tex0Uni, unit);
}
void Texture::Bind() {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(type, ID);
}
void Texture::unBind() { glBindTexture(type, 0); }
void Texture::Delete() { glDeleteTextures(1, &ID); }

} // namespace farixEngine::renderer
