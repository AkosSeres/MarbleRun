#ifndef _PHY3D_SPHERE_MODEL_H_
#define _PHY3D_SPHERE_MODEL_H_

#include <cmath>

#include "Model.h"

class SphereModel : public Model {
 private:
  int sphereResolution;
  float R;

 public:
  SphereModel(int resolution = 12, float radius = 1.0f)
      : Model(), sphereResolution(resolution), R(radius){};
  void setResolution(int newRes) { sphereResolution = newRes; }
  void setRadius(float r) { R = r; }
  int getResolution() const { return sphereResolution; }
  float getRadius() const { return R; }
  void loadModel() override;
  void renderStriped(GLint posAttrib, bool side = false) const;
};

#endif