#ifndef _PHY3D_BALL_H_
#define _PHY3D_BALL_H_

#include <cmath>

#include "Matrix.h"
#include "Vec3.h"

enum BallType { OPAQUE_BALL, SHELL_BALL };

class Ball {
 private:
  Vec3 pos;
  Vec3 vel;
  Matrix orientation;
  Vec3 angVel;
  float r;
  float density;
  float angularMassMultiplier;

 public:
  Ball(const Vec3& pos_ = Vec3(0, 0, 0), float radius = 1);
  float getMass() const { return 4.0f / 3.0f * M_PI * r * r * r; };
  void setType(BallType newType);
  void setDensity(float d) { density = d; };
  void setRadius(float R) { r = R; }
  inline float getAngularMass() const;
  void update(float dt, const Vec3& g = Vec3(0, 0, 0));
  Matrix getModelViewMatrix() const;
};

#endif