
#pragma once
#include <cmath>

namespace farixEngine {
struct Vec3 {
  float x, y, z;

  Vec3();
  Vec3(float a);
  Vec3(float x_, float y_, float z_);

  Vec3 operator+(const Vec3 &v) const;
  Vec3 operator-(const Vec3 &v) const;
  Vec3 operator*(float s) const;
  Vec3 operator/(float s) const;

  bool operator==(const Vec3&v)const;
  float &operator[](int col);
  const float &operator[](int index) const;

  float dot(const Vec3 &v) const;
  Vec3 cross(const Vec3 &v) const;
  float length() const;
  Vec3 normalized() const;
};
} // namespace farixEngine
