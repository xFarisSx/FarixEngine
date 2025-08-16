#include "farixEngine/renderer/opengl/shader.hpp"

#include <glm/gtc/type_ptr.hpp>
namespace farixEngine::renderer {

std::string getFileContents(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Failed to open shader file: " << path << std::endl;
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

Shader::Shader(const std::string &vertexFile, const std::string &fragmentFile) {
  std::string vertexCode = getFileContents(vertexFile);
  std::string fragmentCode = getFileContents(fragmentFile);

  const char *vertexSource = vertexCode.c_str();
  const char *fragmentSource = fragmentCode.c_str();

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);
 
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, &fragmentSource, NULL);
  glCompileShader(fragShader);

  ID = glCreateProgram();

  glAttachShader(ID, vertexShader);
  glAttachShader(ID, fragShader);
  glLinkProgram(ID);

  glDeleteShader(vertexShader);
  glDeleteShader(fragShader);
}

void Shader::Activate() { glUseProgram(ID); }

void Shader::Delete() { glDeleteProgram(ID); }

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
 
void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
}

void Shader::setVec2(const std::string &name, const Vec2 &value) const {
  glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}

void Shader::setVec3(const std::string &name, const Vec3 &value) const {
  glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y,
              value.z);
}

void Shader::setVec4(const std::string &name, const Vec4 &value) const {
  glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z,
              value.w);
}

void Shader::setMat4(const std::string &name, const Mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}

} // namespace farixEngine::renderer
