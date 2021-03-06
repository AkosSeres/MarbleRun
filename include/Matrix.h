/**
 * ©·2021·Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#ifndef _PHY3D_MATRIX_H_
#define _PHY3D_MATRIX_H_

#include <cmath>

/**
 * 4x4 matrix class implemented to be used with OpenGL.
 */
class Matrix {
 private:
  float m[16];

 public:
  Matrix();
  const float* getElements() const { return m; }
  void mult(const Matrix& other);
  static Matrix mult(const Matrix& a, const Matrix& b);
  Matrix operator*(const Matrix& other) const;
  Matrix& operator*=(const Matrix& other);
  Matrix operator*(const float scalar) const;
  Matrix& operator*=(const float scalar);
  void applyTransformation(const Matrix& t) { (*this) = t * (*this); }

  // Recieve different type of matrices with
  // these static functions
  static Matrix scaling(float factor);
  static Matrix scaling(float factorX, float factorY, float factorZ);
  static Matrix translation(float x, float y, float z);
  static Matrix rotationX(float angle);
  static Matrix rotationY(float angle);
  static Matrix rotationZ(float angle);
  static Matrix rotation(float angle, float ux, float uy, float uz);
  static Matrix frustum(float left, float right, float bottom, float top,
                        float zNear, float zFar);
  static Matrix perspective(float fov, float aspectRatio, float zNear,
                            float zFar);
};

#endif
