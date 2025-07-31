
#include "farixEngine/math/vec4.hpp"
#include "farixEngine/math/vec3.hpp"
#include <cmath>
#include <stdexcept>
namespace farixEngine {

Vec4::Vec4() : x(0), y(0), z(0), w(1.0f) {}

Vec4::Vec4(float x_, float y_, float z_, float w_)
    : x(x_), y(y_), z(z_), w(w_) {}

Vec4::Vec4(Vec3 v3, float w_) : x(v3.x), y(v3.y), z(v3.z), w(w_) {}

float &Vec4::operator[](int col) {
  switch (col) {
  case 0:
    return x;
  case 1:
    return y;
  case 2:
    return z;
  case 3:
    return w;
  default:
    throw std::out_of_range("Vec4 index out of range");
  }
}
const float &Vec4::operator[](int index) const {
  switch (index) {
  case 0:
    return x;
  case 1:
    return y;
  case 2:
    return z;
  case 3:
    return w;
  default:
    throw std::out_of_range("Vec4 index out of range");
  }
}

Vec4 Vec4::operator+(const Vec4 &v) const {
  return Vec4(x + v.x, y + v.y, z + v.z);
}

Vec4 Vec4::operator-(const Vec4 &v) const {
  return Vec4(x - v.x, y - v.y, z - v.z);
}

Vec4 Vec4::operator*(float s) const { return Vec4(x * s, y * s, z * s); }
Vec3 Vec4::toVec3() {
  if (w == 0.0f)
    return Vec3(x, y, z);
  return Vec3(x / w, y / w, z / w);
}
  Vec3 Vec4::xyz(){
  return Vec3(x, y, z);
}
float Vec4::dot(const Vec4 &rhs) const {
  return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

} // namespace farixEngine
