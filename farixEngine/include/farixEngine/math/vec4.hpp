
#pragma once
#include "farixEngine/math/vec3.hpp"
#include <cmath>

namespace farixEngine {
struct Vec4 {
  float x, y, z, w;

  Vec4();
  Vec4(float x_, float y_, float z_, float w_ = 1.0f);
  Vec4(Vec3 v3, float w=0);

  float &operator[](int col);
  const float &operator[](int index) const;

  Vec4 operator+(const Vec4 &v) const;
  Vec4 operator-(const Vec4 &v) const;
  Vec4 operator*(float s) const;
  Vec3 toVec3();
  float dot(const Vec4 &rhs) const;
};
} // namespace farixEngine
