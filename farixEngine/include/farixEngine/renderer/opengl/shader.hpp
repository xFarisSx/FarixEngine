#pragma once

#include "farixEngine/thirdparty/glad/glad.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace farixEngine::renderer{

std::string getFileContents(const std::string& path);

class Shader {
public:
  GLuint ID;
  Shader(const std::string &vertexFile, const std::string &fragmentFile) ;
  Shader() = default ;

  void Activate();
  void Delete();
};

}
