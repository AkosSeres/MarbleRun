/**
 * ©·2021·Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include "Camera.h"

// Set default up direction
const Vec3 Camera::up = {0.0f, 1.0f, 0.0f};

/**
 * Initalises the camera.
 */
Camera::Camera() {
  // Just give it some initial values, they can be changed through the setters
  pos = Vec3();
  dir = Vec3(0, 0, -1);
  aspectRatio = 1;
  fov = M_PI_4;
}

/**
 * Makes the camera look at a given point.
 */
void Camera::lookAt(const Vec3& at) {
  Vec3 newDir = at - pos;
  if (newDir.lenSq() == 0.0f) {
    dir = Vec3(0, 0, -1);
    return;
  }
  newDir.setLen(1);
  dir = newDir;
}

/**
 * Tilts the camera upwards by the given angle.
 */
void Camera::tiltUp(float angle) {
  auto right = getRightDir();
  auto forward = getForwardDir();
  float downAngle = Vec3::angle(forward, dir);
  if (dir.y < 0.0f) downAngle = -downAngle;

  float finalAngle = downAngle + angle;
  // Don't let the camera tilt too much
  if (finalAngle < (-M_PI_2 * 0.95)) finalAngle = (-M_PI_2 * 0.95);
  if (finalAngle > (M_PI_2 * 0.95)) finalAngle = (M_PI_2 * 0.95);
  angle = finalAngle - downAngle;
  dir.applyMatrix(Matrix::rotation(angle, right.x, right.y, right.z));
}

/**
 * Returns the horizontal forward direction of the camera.
 */
Vec3 Camera::getForwardDir() const {
  Vec3 forward = dir;
  forward.y = 0;
  forward.setLen(1);
  return forward;
}

/**
 * Returns a projection matrix that can be used in rendering.
 */
Matrix Camera::getProjMatrix(float zNear, float zFar) const {
  Matrix ret;
  auto right = getRightDir();
  auto forward = getForwardDir();
  float downAngle = Vec3::angle(forward, dir);
  if (dir.y < 0.0f) downAngle = -downAngle;
  // Apply the opposite of the camera's orientation to the returned matrix
  ret.applyTransformation(Matrix::translation(-pos.x, -pos.y, -pos.z));
  ret.applyTransformation(
      Matrix::rotation(-downAngle, right.x, right.y, right.z));
  ret.applyTransformation(Matrix::rotationY(M_PI - std::atan2(dir.x, dir.z)));
  ret.applyTransformation(Matrix::perspective(fov, aspectRatio, zNear, zFar));
  return ret;
}
