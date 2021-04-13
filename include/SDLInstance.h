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
 private:
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
  void loadGeometry();
  void initShaders();
  void mainLoop(Uint32 t = 0);

 public:
  SDLInstance(char const* titleStr = NULL);
  void enterLoop();
};

#endif
