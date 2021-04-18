#include "Vec3.h"

/**
 * Returns the length of the considered vector.
 */
float Vec3::len() const { return std::sqrt(x * x + y * y + z * z); }

/**
 * Returns the length squared of the considered vector.
 */
float Vec3::lenSq() const { return (x * x + y * y + z * z); }

/**
 * Sets the length of the vector to the desired value while leaving it's
 * direction unchanged.
 */
Vec3& Vec3::setLen(float newLen) {
  float l = len();
  if (l > 0) mult(newLen / len());
  return (*this);
}

/**
 * Multiplies the vector with a scalar.
 */
Vec3& Vec3::mult(float val) {
  x *= val;
  y *= val;
  z *= val;
  return (*this);
}

/**
 * Divides the vector with a scalar.
 */
Vec3& Vec3::div(float val) {
  x /= val;
  y /= val;
  z /= val;
  return (*this);
}

/**
 * Adds another vector to the vector.
 */
Vec3& Vec3::add(const Vec3& o) {
  x += o.x;
  y += o.y;
  z += o.z;
  return (*this);
}

/**
 * Subtracts another vector to the vector.
 */
Vec3& Vec3::sub(const Vec3& o) {
  x -= o.x;
  y -= o.y;
  z -= o.z;
  return (*this);
}

/**
 * Returns the dot product of the two vectors.
 */
float Vec3::dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }

/**
 * Returns the cross product of the two vectors.
 */
Vec3 Vec3::cross(const Vec3& o) const {
  return Vec3(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
}

/**
 * Rotates the vector around the X axis.
 */
Vec3& Vec3::rotateX(float angle) {
  float sina = std::sin(angle);
  float cosa = std::cos(angle);
  float yO = y;
  y = yO * cosa - z * sina;
  z = yO * sina + z * cosa;
  return (*this);
}

/**
 * Rotates the vector around the Y axis.
 */
Vec3& Vec3::rotateY(float angle) {
  float sina = std::sin(angle);
  float cosa = std::cos(angle);
  float xO = x;
  x = xO * cosa + z * sina;
  z = -xO * sina + z * cosa;
  return (*this);
}

/**
 * Rotates the vector around the Z axis.
 */
Vec3& Vec3::rotateZ(float angle) {
  float sina = std::sin(angle);
  float cosa = std::cos(angle);
  float xO = x;
  x = xO * cosa - y * sina;
  y = xO * sina + y * cosa;
  return (*this);
}

/**
 * Applies the given matrix transformation to the vector.
 */
Vec3& Vec3::applyMatrix(const Matrix& matrix) {
  float a = x, b = y, c = z;
  auto m = matrix.m;

  float w = 1.0f / (m[3] * a + m[7] * b + m[11] * c + m[15]);
  x = (m[0] * a + m[4] * b + m[8] * c + m[12]) * w;
  y = (m[1] * a + m[5] * b + m[9] * c + m[13]) * w;
  z = (m[2] * a + m[6] * b + m[10] * c + m[14]) * w;

  return (*this);
}

/**
 * Static function for vector addition.
 */
Vec3 Vec3::add(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

/**
 * Static function for vector subtraction.
 */
Vec3 Vec3::sub(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vec3 Vec3::mult(const Vec3& v1, float a) {
  return Vec3(v1.x * a, v1.y * a, v1.z * a);
}

/**
 * Static function that returns the dot product of the given vectors.
 */
float Vec3::dot(const Vec3& v1, const Vec3& v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

/**
 * Static function that returns the cross product of two vectors.
 */
Vec3 Vec3::cross(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
              v1.x * v2.y - v1.y * v2.x);
}

/**
 * Returns the angle between the vectors.
 */
float Vec3::angle(const Vec3& v1, const Vec3& v2) {
  float dotp = v1.dot(v2);
  float lens = (v1.len() * v2.len());
  if (lens == 0.0f) return 0.0f;
  dotp /= lens;
  return std::acos(dotp);
}

/**
 * Equals operator.
 */
Vec3& Vec3::operator=(const Vec3& o) {
  (*this).x = o.x;
  (*this).y = o.y;
  (*this).z = o.z;
  return (*this);
}