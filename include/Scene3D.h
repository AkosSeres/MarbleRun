#ifndef _PHY3D_SCENE3D_H_
#define _PHY3D_SCENE3D_H_

#include <cmath>

#include "Camera.h"
#include "Matrix.h"
#include "Model.h"
#include "SDLInstance.h"
#include "Vec3.h"

class Scene3D : public SDLInstance {
 protected:
  void loadGeometry() override;
  void initShaders() override;
  void mainLoop(Uint32 t = 0) override;

  GLint projectionLocation;
  GLint modelViewLocation;
  GLint colorLocation;
  GLint posAttrib;

  Camera cam;
  SphereModel content;
  ObjModel world;

  bool WASDKeys[4];
  bool spaceKey;
  bool shiftKey;
  void keyDownEvent(const SDL_KeyboardEvent& e) override;
  void keyUpEvent(const SDL_KeyboardEvent& e) override;
  void mouseMotionEvent(const SDL_MouseMotionEvent& e) override;

 public:
  Scene3D(char const* titleStr = NULL);
};

#endif
