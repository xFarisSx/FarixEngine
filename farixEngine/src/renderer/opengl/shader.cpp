#include "farixEngine/renderer/opengl/shader.hpp"

namespace farixEngine::renderer{

std::string getFileContents(const std::string& path) {
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

void Shader::Activate()
{
    glUseProgram(ID);
}

void Shader::Delete()
{
    glDeleteProgram(ID);
}
}
