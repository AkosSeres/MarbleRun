#ifndef _PHY3D_VEC3D_H_
#define _PHY3D_VEC3D_H_

#include <cmath>

#include "Matrix.h"

class Vec3 {
 public:
  float x, y, z;  // Make the coordinates publicly accessible because vectors
                  // are well known and easy to handle
  Vec3(float x_ = 0, float y_ = 0, float z_ = 0) : x(x_), y(y_), z(z_) {}
  Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}
  float len() const;
  float lenSq() const;
  Vec3& setLen(float newLen);
  Vec3& mult(float val);
  Vec3& div(float val);
  Vec3& add(const Vec3& o);
  Vec3& sub(const Vec3& o);
  float dot(const Vec3& o) const;
  Vec3 cross(const Vec3& o) const;
  Vec3& rotateX(float angle);
  Vec3& rotateY(float angle);
  Vec3& rotateZ(float angle);
  Vec3& applyMatrix(const Matrix& matrix);

  static Vec3 add(const Vec3& v1, const Vec3& v2);
  static Vec3 sub(const Vec3& v1, const Vec3& v2);
  static Vec3 mult(const Vec3& v1, float a);
  static float dot(const Vec3& v1, const Vec3& v2);
  static Vec3 cross(const Vec3& v1, const Vec3& v2);
  static float angle(const Vec3& v1, const Vec3& v2);

  Vec3 operator+(const Vec3& v) const { return add((*this), v); };
  Vec3 operator-(const Vec3& v) const { return sub((*this), v); };
  Vec3& operator+=(const Vec3& v) { return this->add(v); };
  Vec3& operator-=(const Vec3& v) { return this->sub(v); };
  Vec3 operator-() const { return Vec3(-x, -y, -z); };
  float operator*(const Vec3& v) const { return dot((*this), v); };
  Vec3 operator*(float a) { return Vec3(a * x, a * y, a * z); };
  Vec3& operator*=(float a) { return this->mult(a); };
  Vec3& operator=(const Vec3& o);
};

#endif