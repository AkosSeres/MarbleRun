#ifndef _PHY3D_SCENE3D_H_
#define _PHY3D_SCENE3D_H_

#include <cmath>

#include "Matrix.h"
#include "SDLInstance.h"

class Scene3D : public SDLInstance {
 protected:
  void loadGeometry() override;
  void initShaders() override;
  void mainLoop(Uint32 t = 0) override;

  GLfloat* cubeVertices;
  GLushort* indices;

  GLint projectionLocation;
  GLint modelViewLocation;
  GLint colorLocation;
  GLint posAttrib;
  Matrix projMatrix;
  GLuint elementbuffer;

  bool WASDKeys[4];
  void keyDownEvent(const SDL_KeyboardEvent& e) override;
  void keyUpEvent(const SDL_KeyboardEvent& e) override;

 public:
  Scene3D(char const* titleStr = NULL);
};

#endif
