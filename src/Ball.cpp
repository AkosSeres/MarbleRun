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

    // return;
    Vec3 cp = v;
    Vec3 vRel = -getVelInPos(cp);
    Vec3 t = Vec3::sub(vRel, Vec3::mult(n, n.dot(vRel)));
    t.setLen(1);
    float mass = getMass();
    float angularEffMass = getAngularMass() / (r * r);
    float effMass = 1.0f / ((1.0f / mass) + (1.0f / angularEffMass));
    float dImp = -dv.len() * mass * fc;
    Vec3 fResp;
    if (std::abs(vRel.dot(t) * effMass) <= std::abs(dImp)) {
      fResp = Vec3::mult(t, vRel.dot(t) * effMass);
    } else
      fResp = Vec3::mult(t, -dImp);

    d = Vec3::sub(cp, pos);
    vel.add(Vec3::mult(fResp, 1.0f / mass));
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

/**
 * Tests the collision between two balls and applies response if needed.
 */
void Ball::collide(Ball& b1, Ball& b2) {
  float R = b1.r + b2.r;
  // Return if they do not
  if ((R * R) < (b1.pos - b2.pos).lenSq()) return;
  // Separate the balls
  float m1 = b1.getMass(), m2 = b2.getMass();
  float am1 = b1.getAngularMass(), am2 = b2.getAngularMass();
  Vec3 d = b2.pos - b1.pos;
  d.setLen(R - d.len());
  b2.pos.add(Vec3::mult(d, m1 / (m1 + m2)));
  b1.pos.add(Vec3::mult(d, -m2 / (m1 + m2)));

  // Do not do anything if they are moving away from each other
  Vec3& n = d.setLen(1.0f);
  float v1 = n.dot(b1.vel), v2 = n.dot(b2.vel);
  if (v2 >= v1) return;

  // Calculate collision response
  Vec3 p = b1.pos + (n * b1.r);
  Vec3 vRel1 = b1.getVelInPos(p);
  Vec3 vRel2 = b2.getVelInPos(p);
  Vec3 vRel = vRel2 - vRel1;
  float k = (b1.k + b2.k) * 0.5f;
  Vec3 r1 = p - b1.pos, r2 = p - b2.pos;
  float dImp = Vec3::cross(r1, n).cross(r1).mult(1.0f / am1).dot(n);
  dImp += Vec3::cross(r2, n).cross(r2).mult(1.0f / am2).dot(n);
  dImp += (1.0f / m1);
  dImp += (1.0f / m2);
  dImp = 1.0f / dImp;
  dImp *= (-(1 + k) * Vec3::dot(vRel, n));

  // Modify their velocities accordingly
  b1.vel.sub(Vec3::mult(n, dImp / m1));
  b2.vel.add(Vec3::mult(n, dImp / m2));

  // Deal with friction
  float fc = std::sqrt(b1.fc * b2.fc);
  Vec3 t = Vec3::sub(vRel, n * vRel.dot(n)).setLen(1.0f);
  float amEff1 = am1 / (b1.r * b1.r);
  float amEff2 = am2 / (b2.r * b2.r);
  float effMass1 = 1.0f / ((1.0f / m1) + (1.0f / amEff1));
  float effMass2 = 1.0f / ((1.0f / m2) + (1.0f / amEff2));
  float effMass = 1.0f / ((1.0f / effMass1) + (1.0f / effMass2));
  float dResp = fc * dImp;
  Vec3 fResp;
  if (std::abs(vRel.dot(t) * effMass) <= std::abs(dImp)) {
    fResp = Vec3::mult(t, -vRel.dot(t) * effMass);
  } else
    fResp = Vec3::mult(t, -dImp);

  // Modify the velocities accordig to the friction impulse
  b1.vel.sub(Vec3::mult(fResp, 1.0f / m1));
  b2.vel.add(Vec3::mult(fResp, 1.0f / m2));
  b1.angVel.sub(r1.cross(fResp) * (1.0f / am1));
  b2.angVel.add(r2.cross(fResp) * (1.0f / am2));
}