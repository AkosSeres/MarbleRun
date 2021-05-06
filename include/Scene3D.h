#ifndef _PHY3D_SCENE3D_H_
#define _PHY3D_SCENE3D_H_

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Ball.h"
#include "Camera.h"
#include "Matrix.h"
#include "Model.h"
#include "ObjModel.h"
#include "SDLInstance.h"
#include "SphereModel.h"
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
  Ball* balls;
  int ballCount;

  bool WASDKeys[4];
  bool spaceKey;
  bool shiftKey;
  bool timeStopped;
  void keyDownEvent(const SDL_KeyboardEvent& e) override;
  void keyUpEvent(const SDL_KeyboardEvent& e) override;
  void mouseMotionEvent(const SDL_MouseMotionEvent& e) override;
  void mouseButtonDownEvent(const SDL_MouseButtonEvent& e) override;
  void resizeEvent(int width, int height) override;
  void fileDropEvent(const char* fName) override;

  void placeBall();
  void addBall(const Ball& b);

 public:
  Scene3D(char const* titleStr = NULL);
  ~Scene3D();
  void saveScene(const char* fileName) const;
  friend std::ostream& operator<<(std::ostream& os, const Scene3D& scene);
  friend std::istream& operator>>(std::istream& is, Scene3D& scene);
};

#endif
