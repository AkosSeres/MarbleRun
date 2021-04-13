#include "Matrix.h"

/**
 * Creates an identity matrix
 */
Matrix::Matrix() {
  m[0] = m[5] = m[10] = m[15] = 1.0f;
  m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] =
      m[13] = m[14] = 0.0f;
}

/**
 * Multiplies the matrix with another one
 */
void Matrix::mult(const Matrix& other) {
  float temp[16];
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      temp[4 * i + j] = m[j] * other.m[4 * i] + m[4 + j] * other.m[4 * i + 1] +
                        m[8 + j] * other.m[4 * i + 2] +
                        m[12 + j] * other.m[4 * i + 3];

  for (int i = 0; i < 16; i++) m[i] = temp[i];
}

/**
 * Multiplies two matrices and returns the result
 */
static Matrix& mult(const Matrix& a, const Matrix& b) {
  Matrix ret;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      ret.m[4 * i + j] = a.m[j] * b.m[4 * i] + a.m[4 + j] * b.m[4 * i + 1] +
                         a.m[8 + j] * b.m[4 * i + 2] +
                         a.m[12 + j] * b.m[4 * i + 3];

  return ret;
}

/**
 * Multiply operator for multiplying matrices
 */
Matrix& Matrix::operator*(const Matrix& other) const {
  return Matrix::mult((*this), other);
}

/**
 * Multiply equals operator for multiplying matrices
 */
Matrix& Matrix::operator*=(const Matrix& other) {
  mult(other);
  return (*this);
}

/**
 * Multiply operator for multiplying the matrix with a scalar
 */
Matrix& Matrix::operator*(const float scalar) const {
  Matrix ret = (*this);
  for (int i = 0; i < 16; i++) ret.m[i] *= scalar;
  return ret;
}

/**
 * Multiply equals operator for multiplying the matrix with a scalar value
 */
Matrix& Matrix::operator*=(const float scalar) {
  for (int i = 0; i < 16; i++) m[i] *= scalar;
  return (*this);
}

/**
 * Returns a matrix with a scaling transformation with the same scaling factor
 * along all the axes
 */
Matrix& Matrix::scaling(float factor) {
  Matrix ret;
  ret.m[0] = ret.m[5] = ret.m[10] = factor;
  return ret;
}

/**
 * Returns a matrix with a scaling transformation with the given scaling factors
 */
Matrix& Matrix::scaling(float factorX, float factorY, float factorZ) {
  Matrix ret;
  ret.m[0] = factorX;
  ret.m[5] = factorY;
  ret.m[10] = factorZ;
  return ret;
}

/**
 * Returns a translation transformation matrix with the given offsets
 */
Matrix& Matrix::translation(float x, float y, float z) {
  Matrix ret;
  ret.m[12] = x;
  ret.m[13] = y;
  ret.m[14] = z;
  return ret;
}

/**
 * Returns a rotation transformation matrix around the x axis by the given angle
 */
Matrix& Matrix::rotationX(float angle) {
  Matrix ret;
  float cosa = std::cos(angle);
  float sina = std::sin(angle);
  ret.m[5] = cosa;
  ret.m[6] = sina;
  ret.m[9] = -sina;
  ret.m[10] = cosa;
  return ret;
}

/**
 * Returns a rotation transformation matrix around the y axis by the given angle
 */
Matrix& Matrix::rotationY(float angle) {
  Matrix ret;
  float cosa = std::cos(angle);
  float sina = std::sin(angle);
  ret.m[0] = cosa;
  ret.m[2] = -sina;
  ret.m[8] = sina;
  ret.m[10] = cosa;
  return ret;
}

/**
 * Returns a rotation transformation matrix around the z axis by the given angle
 */
Matrix& Matrix::rotationZ(float angle) {
  Matrix ret;
  float cosa = std::cos(angle);
  float sina = std::sin(angle);
  ret.m[0] = cosa;
  ret.m[1] = sina;
  ret.m[4] = -sina;
  ret.m[5] = cosa;
  return ret;
}

/**
 * Returns a rotation transformation matrix around the given axis by the given
 * angle
 */
Matrix& Matrix::rotation(float angle, float ux, float uy, float uz) {
  Matrix ret;
  float cosa = std::cos(angle);
  float sina = std::sin(angle);
  float mcosa = 1.0f - cosa;
  ret.m[0] = cosa + ux * ux * mcosa;
  ret.m[1] = uy * ux * mcosa + uz * sina;
  ret.m[2] = uz * ux * mcosa - uy * sina;
  ret.m[4] = ux * uy * mcosa - uz * sina;
  ret.m[5] = cosa + uy * uy * mcosa;
  ret.m[6] = uz * uy * mcosa + ux * sina;
  ret.m[8] = ux * uz * mcosa + uy * sina;
  ret.m[9] = uy * uz * mcosa - ux * sina;
  ret.m[10] = cosa + uz * uz * mcosa;
  return ret;
}

Matrix& Matrix::frustum(float left, float right, float bottom, float top,
                        float zNear, float zFar) {
  float temp, xDist, yDist, zDist;
  temp = 2.0 * zNear;
  xDist = right - left;
  yDist = top - bottom;
  zDist = zFar - zNear;
  Matrix ret;
  ret.m[0] = temp / xDist;
  ret.m[5] = temp / yDist;
  ret.m[8] = (right + left) / xDist;
  ret.m[9] = (top + bottom) / yDist;
  ret.m[10] = (-zFar - zNear) / zDist;
  ret.m[11] = -1.0f;
  ret.m[14] = (-temp * zFar) / zDist;
  ret.m[15] = 0.0f;
  return ret;
}

Matrix& Matrix::perspective(float fov, float aspectRatio, float zNear,
                            float zFar) {
  float ymax, xmax;
  ymax = zNear * tanf(fov);
  xmax = ymax * aspectRatio;
  return Matrix::frustum(-xmax, xmax, -ymax, ymax, zNear, zFar);
}
