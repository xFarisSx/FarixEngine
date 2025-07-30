#pragma once

#include "farixEngine/math/vec4.hpp"
#include <cmath>

namespace farixEngine {
struct TransformComponent;
struct Mat4 {
  float m[4][4]; // column-major: m[column][row]

  Mat4(); // identity mat

  // Access: m[col][row]
  float *operator[](int col);
  const float *operator[](int col) const;

  // Matrix × Matrix
  Mat4 operator*(const Mat4 &rhs) const;

  // Matrix × Vec4
  Vec4 operator*(const Vec4 &v) const;

  Mat4 transpose() const;

  Vec4 getRow(int r) const;
  Vec4 getCol(int c) const;

  // Static factory methods
  static Mat4 identity() { return Mat4(); }

  static Mat4 translate(const Vec3 &v) {
    Mat4 tm{};
    tm[3][0] = v.x;
    tm[3][1] = v.y;
    tm[3][2] = v.z;

    return tm;
  }

  static Mat4 scale(const Vec3 &v) {
    Mat4 sm{};
    sm[0][0] = v.x;
    sm[1][1] = v.y;
    sm[2][2] = v.z;

    return sm;
  }

  static Mat4 rotateX(float angle) {
    Mat4 rx{};
    rx[1][1] = std::cos(angle);
    rx[2][2] = std::cos(angle);
    rx[1][2] = std::sin(angle);
    rx[2][1] = -std::sin(angle);

    return rx;
  }
  static Mat4 rotateY(float angle) {
    Mat4 ry{};
    ry[0][0] = std::cos(angle);
    ry[2][2] = std::cos(angle);
    ry[2][0] = std::sin(angle);
    ry[0][2] = -std::sin(angle);

    return ry;
  }
  static Mat4 rotateZ(float angle) {
    Mat4 rz{};
    rz[0][0] = std::cos(angle);
    rz[1][1] = std::cos(angle);
    rz[0][1] = std::sin(angle);
    rz[1][0] = -std::sin(angle);

    return rz;
  }

  static Mat4 rotationXYZ(Vec3 r) {
    return rotateZ(r.z) * rotateY(r.y) * rotateX(r.x);
  }

  static Mat4 modelMatrix(const TransformComponent &transform);

  static Mat4 perspective(float fov, float aspect, float near, float far) {
    Mat4 pm{};

    // pm[0][0] = 1 / (aspect * std::tan(fov / 2));
    // pm[1][1] = 1 / (std::tan(fov / 2));
    // pm[2][2] = far / (far - near);
    // pm[2][3] = 1;
    // pm[3][3] = 0;
    // pm[3][2] = -(far * near) / (far - near);

    pm[0][0] = 1 / (aspect * std::tan(fov / 2));
    pm[1][1] = 1 / std::tan(fov / 2);
    pm[2][2] = -(far + near) / (far - near);
    pm[2][3] = -1;
    pm[3][2] = -(2 * far * near) / (far - near);
    pm[3][3] = 0;

    return pm;
  }
  static Mat4 lookAt(const Vec3 &eye, const Vec3 &target, const Vec3 &rup) {
    Vec3 forward = (target-eye).normalized() ;
    Vec3 right = forward.cross(rup).normalized();
    Vec3 up = right.cross(forward).normalized();

    Mat4 vm{};
    vm[0][0] = right.x;
    vm[1][0] = right.y;
    vm[2][0] = right.z;
    vm[3][0] = -1 * right.dot(eye);
    vm[0][1] = up.x;
    vm[1][1] = up.y;
    vm[2][1] = up.z;
    vm[3][1] = -1 * up.dot(eye);
    vm[0][2] = -forward.x;
    vm[1][2] = -forward.y;
    vm[2][2] = -forward.z;
    vm[3][2] =  forward.dot(eye);

    return vm;
  }

  static Mat4 ortho(float left, float right, float bottom, float top,
                    float nearZ, float farZ) {
    Mat4 m = Mat4::identity();

    m[0][0] = 2.0f / (right - left);
    m[1][1] = 2.0f / (top - bottom);
    m[2][2] = -1.0f / (farZ - nearZ);
    m[3][0] = -(right + left) / (right - left);
    m[3][1] = -(top + bottom) / (top - bottom);
    m[3][2] = -nearZ / (farZ - nearZ);

    return m;

  }
};
} // namespace farixEngine
