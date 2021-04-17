#ifndef _PHY3D_SDLINSTANCE_H_
#define _PHY3D_SDLINSTANCE_H_

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include <iostream>

#include "Shaders.h"

class SDLInstance {
 protected:
  SDL_Window* window;
  SDL_GLContext glContext;
  SDL_Renderer* renderer;
  GLuint vertexBufferObj;
  bool isRunning = false;

  GLuint vertexShader;
  GLuint pixelShader;
  GLuint shaderProgram;
  GLuint timeLocation;

  void handleEvents();
  void initWindow(char const* titleStr = NULL, int w = 500, int h = 500);
  virtual void loadGeometry();
  virtual void initShaders();
  virtual void mainLoop(Uint32 t = 0);

  // Event callbacks
  virtual void keyDownEvent(const SDL_KeyboardEvent& e){};
  virtual void keyUpEvent(const SDL_KeyboardEvent& e){};
  virtual void mouseMotionEvent(const SDL_MouseMotionEvent& e){};
  virtual void windowEvent(const SDL_WindowEvent& e){};

 public:
  SDLInstance(char const* titleStr = NULL);
  void enterLoop();
};

#endif
