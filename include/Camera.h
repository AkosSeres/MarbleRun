#ifndef _PHY3D_CAMERA_H_
#define _PHY3D_CAMERA_H_

#include <cmath>

#include "Matrix.h"
#include "Vec3.h"

class Camera {
 private:
  Vec3 pos;  // The position of the camera
  Vec3 dir;  // The looking direction of the camera
  float aspectRatio;
  float fov;
  static const Vec3 up;

 public:
  Camera();
  Vec3 getPos() const { return pos; }
  void setPos(const Vec3& newPos) { pos = newPos; }
  void moveBy(const Vec3& d) { pos += d; }
  Vec3 getDir() const { return dir; }
  void setDir(const Vec3& newDir) { (dir = newDir).setLen(1); }
  void lookAt(const Vec3& at);
  void setAspectRatio(float newAr) { aspectRatio = newAr; }
  void setFOV(float newFov) { fov = newFov; }
  void turnRight(float angle) { dir.rotateY(-angle); }
  void turnLeft(float angle) { dir.rotateY(angle); }
  void tiltUp(float angle);
  Vec3 getRightDir() const { return dir.cross(up); }
  Vec3 getForwardDir() const;
  Matrix getProjMatrix(float zNear, float zFar) const;
};

#endif