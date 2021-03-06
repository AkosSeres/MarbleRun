/**
 * ©·2021·Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#ifndef _PHY3D_BALL_H_
#define _PHY3D_BALL_H_

#include <cmath>

#include "Matrix.h"
#include "Model.h"
#include "Vec3.h"

enum BallType { OPAQUE_BALL, SHELL_BALL };

/**
 * Class that describes a ball that can move and collide with static geometry
 * and other balls.
 */
class Ball {
 private:
  Vec3 pos;            // The position of the ball
  Vec3 vel;            // The velocity of the ball
  Matrix orientation;  // The orientation of the ball stored as a matrix
  Vec3 angVel;         // The angular velocity of the ball (vector value)
  float r;             // The radius of the ball
  float density;       // The density of the ball
  float
      angularMassMultiplier;  // This describes the inner structure of the ball
  float k;                    // Coefficent of restitution
  float fc;                   // Frction coefficient

 public:
  Ball(const Vec3& pos_ = Vec3(0, 0, 0), float radius = 1);
  float getMass() const { return 4.0f / 3.0f * M_PI * r * r * r; };
  void setType(BallType newType);
  void setPosition(const Vec3& p) { pos = p; };
  Vec3 getPosition() const { return pos; };
  void setVel(const Vec3& v) { vel = v; }
  Vec3 getVel() const { return vel; }
  void setDensity(float d) { density = d; };
  float getDensity() const { return density; };
  void setRadius(float R) { r = R; }
  float getRadius() const { return r; }
  void setBounciness(float k_);
  float getBounciness() const { return k; };
  void setFrictionCoefficient(float fc_) { fc = std::abs(fc_); };
  float getFrictionCoefficient() const { return fc; };
  inline float getAngularMass() const;
  void setAngularMassMultiplier(float newAmm) {
    angularMassMultiplier = std::abs(newAmm);
  };
  float getAngularMassMultiplier() const { return angularMassMultiplier; };
  void update(float dt, const Vec3& g = Vec3(0, 0, 0));
  Matrix getModelViewMatrix() const;
  Vec3 getVelInPos(const Vec3& p) const;
  void collideWithModel(const Model& m);

  static void collide(Ball& b1, Ball& b2);
};

#endif