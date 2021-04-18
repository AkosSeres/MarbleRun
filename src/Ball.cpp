#include "Ball.h"

/**
 * Initalises a ball.
 */
Ball::Ball(const Vec3& pos_, float radius) {
  pos = pos_;
  r = radius;
  vel = Vec3(0, 0, 0);
  orientation = Matrix();
  angVel = Vec3(0, 0, 0);
  density = 1;
  // By default set the ball as an opaque sphere
  setType(OPAQUE_BALL);
}

/**
 * Sets the type of the ball - is it a full or hollow sphere?
 */
void Ball::setType(BallType newType) {
  switch (newType) {
    case OPAQUE_BALL:
      angularMassMultiplier = 2.0f / 5.0f;
      break;
    case SHELL_BALL:
      angularMassMultiplier = 2.0f / 3.0f;
      break;
    default:
      angularMassMultiplier = 2.0f / 5.0f;
      break;
  }
}

/**
 * Returns the angular mass of the ball.
 */
inline float Ball::getAngularMass() const {
  return r * r * angularMassMultiplier * getMass();
}

/**
 * Updates the ball by the given elapsed time. It moves and rotates it
 * accordingly to its velocities and the given gravity. I will maybe implement
 * something like air drag later.
 */
void Ball::update(float dt, const Vec3& g) {
  pos += (vel * dt);
  vel += (g * dt);
  Vec3 rotAx = angVel;
  rotAx.setLen(1.0f);
  orientation.applyTransformation(
      Matrix::rotation(angVel.len() * dt, rotAx.x, rotAx.y, rotAx.z));
}

/**
 * Returns the model view matrix of the ball. By applying this matrix on a
 * 1-radius sphere, the ball can be rendered easily.
 */
Matrix Ball::getModelViewMatrix() const {
  Matrix ret = orientation;
  ret.applyTransformation(Matrix::scaling(r));
  ret.applyTransformation(Matrix::translation(pos.x, pos.y, pos.z));
  return ret;
}