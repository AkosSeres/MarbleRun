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
  k = 0.2;
  fc = 0.2;
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
 * Sets the coefficient of restitution of the ball.
 */
void Ball::setBounciness(float k_) {
  if (k_ < 0.0f)
    k = 0.0f;
  else if (k > 1.0f)
    k = 1.0f;
  else
    k = k_;
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
  Vec3 dx = Vec3::mult(vel, dt);
  pos = pos + dx;
  Vec3 dv = Vec3::mult(g, dt);
  vel += dv;
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
  Matrix ret;
  ret.applyTransformation(orientation);
  ret.applyTransformation(Matrix::scaling(r));
  ret.applyTransformation(Matrix::translation(pos.x, pos.y, pos.z));
  return ret;
}

/**
 * Returns the ball's velocity in the given position.
 */
Vec3 Ball::getVelInPos(const Vec3& p) const {
  Vec3 rel = Vec3::sub(p, pos);
  return (angVel.cross(rel) + vel);
}

/**
 * Tests collision against the given static model and applies the appropriate
 * collision response.
 */
void Ball::collideWithModel(const Model& m) {
  auto collideWithPoint = [&](const Vec3& v) {
    Vec3 d = Vec3::sub(v, pos);
    if (d.lenSq() > (r * r)) return;
    if ((vel.dot(d)) <= 0) return;
    // Separate the bodies
    d.setLen(r - d.len());
    pos.sub(d);
    // Calculate collision normal
    Vec3 n = Vec3::sub(v, pos).setLen(1);
    // Calculate change in velocity
    Vec3 dv = n * (vel.dot(n)) * (1 + k);
    vel.sub(dv);

    Vec3 cp = v;
    Vec3 vRel = -getVelInPos(cp);
    Vec3 t = Vec3::sub(vRel, Vec3::mult(n, n.dot(vRel)));
    t.setLen(1);
    float dImp = -dv.len() * getMass() * fc;
    Vec3 fResp;
    if (vRel.dot(t) * getMass() <= dImp) {
      fResp = Vec3::mult(t, -vRel.dot(t) * getMass());
    } else
      fResp = Vec3::mult(t, -dImp);

    d = Vec3::sub(cp, pos);
    vel.add(Vec3::mult(fResp, 1 / getMass()));
    angVel.add(Vec3::mult(d.cross(fResp), 1 / getAngularMass()));
  };

  GLuint vNum = m.getVertexNum();
  for (int i = 0; i < vNum; i++) {
    collideWithPoint(m.getVertex(i));
  }

  GLuint tNum = m.getTriangleNum();
  for (int i = 0; i < tNum; i++) {
    Vec3 a, b, c;
    m.getTriangle(i, &a, &b, &c);
    Vec3 AB = b - a;
    Vec3 BC = c - b;
    Vec3 CA = a - c;
    Vec3 AC = c - a;
    Vec3 n = Vec3::cross(AB, AC);
    Vec3 aRel = pos - a;
    Vec3 bRel = pos - b;
    Vec3 cRel = pos - c;
    Vec3 aPerp = n.cross(AB);
    Vec3 bPerp = n.cross(BC);
    Vec3 cPerp = n.cross(CA);
    if (Vec3::dot(aRel, aPerp) >= 0 && Vec3::dot(bRel, bPerp) >= 0 &&
        Vec3::dot(cRel, cPerp) >= 0) {
      n.setLen(1);
      n.mult(n.dot(aRel));
      Vec3 cp = Vec3::sub(pos, n);
      collideWithPoint(cp);
    }
  }
}
