#include "SDLInstance.h"

SDLInstance::SDLInstance(char const* titleStr) {
  initWindow(titleStr);
  this->loadGeometry();
  this->initShaders();
}

#ifdef __EMSCRIPTEN__
namespace WidthGetter {
EM_JS(int, getBrowserWidth, (), { return window.innerWidth; });
EM_JS(int, getBrowserHeight, (), { return window.innerHeight; });
}  // namespace WidthGetter
#endif

void SDLInstance::initWindow(char const* titleStr, int w, int h) {
  if (titleStr == NULL) titleStr = "";

#ifdef __EMSCRIPTEN__
  w = WidthGetter::getBrowserWidth();
  h = WidthGetter::getBrowserHeight();
#endif
  width = w;
  height = h;
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  // Create SDL window
  window =
      SDL_CreateWindow(titleStr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  // Set OpenGL related attributes
  // OpenGL ES has to be used to retain browser compatibility
  // It's harder this way but I like when my projects can be quickly
  // presented by opening a browser on virtually any device
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetSwapInterval(0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  // Initalise OpenGL
  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
}

void SDLInstance::loadGeometry() {
  // Load the vertices into the vertex buffer
  GLuint vertexBufferObj;
  glGenBuffers(1, &vertexBufferObj);

  GLfloat vertices[] = {0.0f,  0.5f,  0.5f, 0.0f, 0.0f,
                        -0.5f, -0.5f, 0.0f, 0.0f, 0.5f};

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void SDLInstance::initShaders() {
  // Load vertex shader
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &Shaders::squishVertex, NULL);
  glCompileShader(vertexShader);

  // Load pixel shader
  pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(pixelShader, 1, &Shaders::baseFragment, NULL);
  glCompileShader(pixelShader);

  // Link the shaders into a program and tell
  // OpenGL to use it
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, pixelShader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  // This is just a uniform that gives the shader the time
  // so the shader can apply a simple effect just to show that
  // everything is working
  timeLocation = glGetUniformLocation(shaderProgram, "time");

  // Tell OpenGL the format of vertices
  auto posAttrib = glGetAttribLocation(shaderProgram, "pos");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

/** Main loop lambda function has to be wrapped inside a regular function to be
 * able to be called by emscripten **/
#ifdef __EMSCRIPTEN__
#include <functional>
namespace LoopCaller {
std::function<void()> mainLoopFunc;
void mainLoop() { mainLoopFunc(); };
}  // namespace LoopCaller
#endif

void SDLInstance::enterLoop() {
  isRunning = true;

  // The main loop behaviour is inside a lambda function
  // Probably not the best method but the easiest right now
  auto loopFunc = [this] {
    // Run the event handler
    handleEvents();

    // Call the main loop behavoiur function
    mainLoop(SDL_GetTicks());

    SDL_GL_SwapWindow(window);
  };

#ifdef __EMSCRIPTEN__
  // Setting main loop lambda function
  LoopCaller::mainLoopFunc = loopFunc;
  // Then set the main loop function
  emscripten_set_main_loop(LoopCaller::mainLoop, 0, true);
#else
  // In native mode just use a while loop
  while (isRunning) loopFunc();
#endif
}

void SDLInstance::mainLoop(Uint32 t) {
  // Give the time to the shader
  glUniform1f(timeLocation, (float)t);

  // Set background color (black)
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the triangles
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glDrawArrays(GL_TRIANGLES, 2, 3);
}

void SDLInstance::handleEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT:
        isRunning = false;
        break;
      case SDL_KEYDOWN:
        keyDownEvent(e.key);
        break;
      case SDL_KEYUP:
        keyUpEvent(e.key);
        break;
      case SDL_MOUSEMOTION:
        mouseMotionEvent(e.motion);
        break;
      case SDL_MOUSEBUTTONDOWN:
        mouseButtonDownEvent(e.button);
        break;
      case SDL_MOUSEBUTTONUP:
        mouseButtonUpEvent(e.button);
        break;
      case SDL_DROPFILE:
        fileDropEvent(e.drop.file);
        SDL_free(e.drop.file);
        break;
      case SDL_WINDOWEVENT:
        // Store the new size when the window is resized
        // (also call the corresponding event function if is watched)
        if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
          width = e.window.data1;
          height = e.window.data2;
          resizeEvent(e.window.data1, e.window.data2);
        }
        break;

        // Handle custom events
      case SDL_USEREVENT:
        switch (e.user.code) {
#ifdef __EMSCRIPTEN__
            // Specific to the WASM build, because window resizing and the
            // scaling of the canvas is handled in JS
            // And also the event is fired from JS
          case 1:
            width = *((int*)(e.user.data1));
            height = *((int*)(e.user.data2));
            resizeEvent(width, height);
            break;
#endif
        }
        break;
      default:
        break;
    }
  }
}

#ifdef __EMSCRIPTEN__
extern "C" {
/**
 * This function is called from JS when a file is dropped on the canvas in the
 * browser, recieves the file name of the dropped file
 * The file is already loaded into the virtual file system emulated by
 * emscripten when the function is called
 */
int SDLEv_dropEventForSDL(char* fileName) {
  SDL_Event* e = new SDL_Event;
  SDL_DropEvent event;
  // Set the members of the event to push
  event.type = SDL_DROPFILE;
  event.timestamp = SDL_GetTicks();
  size_t memLen = strlen(fileName) + 1;
  event.file = (char*)SDL_malloc(memLen);
  memcpy(event.file, fileName, memLen);
  event.windowID = 0;
  e->type = SDL_DROPFILE;
  e->drop = event;
  // Push event to the SDL event queue
  // This is the function that SDL uses to add new events to the queue to be
  // fetched
  int retVal = SDL_PushEvent(e);
  delete e;
  return retVal;
}

/**
 * This function gets called from JS when the browser window is resized and the
 * canvas has to be resized too to fit the new size
 * It fires a custom event for handling resizing in the WASM build
 * Note that if an SDL_WINDOWEVENT is pushed, it gets rejected by SDL so a
 * custom event type has to be utilized
 */
int SDLEv_browserWasResized() {
  SDL_Event* e = new SDL_Event;
  SDL_UserEvent event;
  // Set members of the custom event
  event.type = SDL_USEREVENT;
  int width = WidthGetter::getBrowserWidth();
  int height = WidthGetter::getBrowserHeight();
  // The new browser window size is stored int data1 and data2
  event.data1 = &width;
  event.data2 = &height;
  event.timestamp = SDL_GetTicks();
  event.windowID = 1;
  event.code = 1;
  e->type = SDL_USEREVENT;
  e->user = event;
  // And finally push the event to the event queue
  int retVal = SDL_PushEvent(e);
  delete e;
  return retVal;
}
}
#endif