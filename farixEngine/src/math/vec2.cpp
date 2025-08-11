
#include "farixEngine/math/vec2.hpp"
#include <cmath>
#include <stdexcept>
namespace farixEngine {

Vec2::Vec2() : x(0), y(0) {}
Vec2::Vec2(float a) : x(a), y(a) {}
Vec2::Vec2(float x_, float y_) : x(x_), y(y_) {}

Vec2 Vec2::operator+(const Vec2 &v) const {
  return Vec2(x + v.x, y + v.y);
}

Vec2 Vec2::operator-(const Vec2 &v) const {
  return Vec2(x - v.x, y - v.y);
}

Vec2 Vec2::operator*(float s) const { return Vec2(x * s, y * s); }

Vec2 Vec2::operator/(float s) const { return Vec2(x /s, y / s); }

bool Vec2::operator==(const Vec2&v)const{
  return (x == v.x && y== v.y);
}
float Vec2::length() const { return std::sqrt(x * x + y * y ); }

Vec2 Vec2::normalized() const {
  float len = length();
  if (len == 0)
    return Vec2(0, 0);
  return Vec2(x / len, y / len);
}

float &Vec2::operator[](int col) {
  switch (col) {
  case 0:
    return x;
  case 1:
    return y;
  default:
    throw std::out_of_range("Vec3 index out of range");
  }
}
const float &Vec2::operator[](int index) const {
  switch (index) {
  case 0:
    return x;
  case 1:
    return y;
  default:
    throw std::out_of_range("Vec3 index out of range");
  }
}

} // namespace farixEngine
