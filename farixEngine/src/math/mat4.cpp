#include "farixEngine/math/mat4.hpp"
#include "farixEngine/components/components.hpp"
namespace farixEngine {

Mat4::Mat4() {
  for (int c = 0; c < 4; c++)
    for (int r = 0; r < 4; r++)
      m[c][r] = (c == r) ? 1.0f : 0.0f;
}

float *Mat4::operator[](int col) { return m[col]; }
const float *Mat4::operator[](int col) const { return m[col]; }

Vec4 Mat4::getRow(int r) const {
  return Vec4(m[0][r], m[1][r], m[2][r], m[3][r]);
}
Vec4 Mat4::getCol(int c) const {
  return Vec4(m[c][0], m[c][1], m[c][2], m[c][3]);
}

Mat4 Mat4::operator*(const Mat4 &rhs) const {
  Mat4 nm{};
  for (int c = 0; c < 4; c++) {
    Vec4 col = rhs.getCol(c);
    for (int r = 0; r < 4; r++) {
      float sum = getRow(r).dot(col);

      nm[c][r] = sum;
    }
  }

  return nm;
}

Vec4 Mat4::operator*(const Vec4 &v) const {
  Vec4 result{};
  for (int r = 0; r < 4; r++) {
    result[r] = getRow(r).dot(v);
  }

  return result;
}

Mat4 Mat4::transpose() const {
  Mat4 nm{};
  for (int c = 0; c < 4; c++)
    for (int r = 0; r < 4; r++)
      nm[c][r] = m[r][c];

  return nm;
}

Mat4 Mat4::modelMatrix(const TransformComponent &transform){
    Mat4 mm = Mat4::translate(transform.position) *
              Mat4::rotationXYZ(transform.rotation) *
              Mat4::scale(transform.scale);

    return mm;
  }

}; // namespace farixEngine
