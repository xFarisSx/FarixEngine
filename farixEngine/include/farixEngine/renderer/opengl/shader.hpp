#pragma once

#include "farixEngine/math/mat4.hpp"
#include "farixEngine/math/vec2.hpp"
#include "farixEngine/math/vec3.hpp"
#include "farixEngine/math/vec4.hpp"
#include "farixEngine/thirdparty/glad/glad.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace farixEngine::renderer {

std::string getFileContents(const std::string &path);

class Shader {
public:
  GLuint ID;
  Shader(const std::string &vertexFile, const std::string &fragmentFile);
  Shader() = default;

  void Activate();
  void Delete();

  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setVec2(const std::string &name, const Vec2 &value) const;
  void setVec3(const std::string &name, const Vec3 &value) const;
  void setVec4(const std::string &name, const Vec4 &value) const;
  void setMat4(const std::string &name, const Mat4 &mat) const;
};

} // namespace farixEngine::renderer
