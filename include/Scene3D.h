/**
 * ©·2021·Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#ifndef _PHY3D_SCENE3D_H_
#define _PHY3D_SCENE3D_H_

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Ball.h"
#include "Camera.h"
#include "Matrix.h"
#include "Model.h"
#include "ObjModel.h"
#include "Shaders.h"
#include "SphereModel.h"
#include "Vec3.h"

class Scene3D {
 private:
  SDL_Window* window;
  SDL_GLContext glContext;
  SDL_Renderer* renderer;
  GLuint vertexBufferObj;
  bool isRunning = false;

  int width, height;

  GLuint vertexShader;
  GLuint pixelShader;
  GLuint shaderProgram;

  void initWindow(char const* titleStr = NULL, int w = 500, int h = 500);
  void loadGeometry();
  void initShaders();
  void mainLoop(Uint32 t = 0);

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

  void handleEvents();
  void keyDownEvent(const SDL_KeyboardEvent& e);
  void keyUpEvent(const SDL_KeyboardEvent& e);
  void mouseMotionEvent(const SDL_MouseMotionEvent& e);
  void mouseButtonDownEvent(const SDL_MouseButtonEvent& e);
  void resizeEvent(int width, int height);
  void fileDropEvent(const char* fName);

#ifdef __EMSCRIPTEN__
  static void getBrowserDimensions(int* width, int* height);
#endif

  void placeBall();
  void addBall(const Ball& b);
  void clearBalls();

 public:
  Scene3D(char const* titleStr = NULL);
  ~Scene3D();
  void enterLoop();
  void saveScene(const char* fileName) const;
  friend std::ostream& operator<<(std::ostream& os, const Scene3D& scene);
  friend std::istream& operator>>(std::istream& is, Scene3D& scene);
};

#endif
