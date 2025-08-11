
#pragma once
#include <cmath>

namespace farixEngine {
struct Vec2 {
  float x, y;

  Vec2();
  Vec2(float a);
  Vec2(float x_, float y_);

  Vec2 operator+(const Vec2 &v) const;
  Vec2 operator-(const Vec2 &v) const;
  Vec2 operator*(float s) const;
  Vec2 operator/(float s) const;

  bool operator==(const Vec2&v)const;


  float &operator[](int col);
  const float &operator[](int index) const;

  float length() const;
  Vec2 normalized() const;
};
} // namespace farixEngine
