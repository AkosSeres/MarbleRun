/**
 * ©·2021·Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include "Scene3D.h"

#ifdef __EMSCRIPTEN__
/**
 * Functions for retrieving the dimensions of the browser window when running in
 * the browser.
 */
namespace WidthGetter {
EM_JS(int, getBrowserWidth, (), { return window.innerWidth; });
EM_JS(int, getBrowserHeight, (), { return window.innerHeight; });
}  // namespace WidthGetter

/**
 * Returns the width and height of the browser window.
 */
void Scene3D::getBrowserDimensions(int* width, int* height) {
  (*width) = WidthGetter::getBrowserWidth();
  (*height) = WidthGetter::getBrowserHeight();
}
#endif

/**
 * Initalises the application.
 * Cerates the window, loads the base scene and the shaders used for rendering.
 */
Scene3D::Scene3D(char const* titleStr) : content((initWindow(titleStr), 18)) {
  // The variables indicating some button states
  WASDKeys[0] = WASDKeys[1] = WASDKeys[2] = WASDKeys[3] = spaceKey = shiftKey =
      timeStopped = false;

  // Set the field of view of the camera as well as the aspect ratio
  try {
    cam.setFOV((M_PI / 1.75f));
  } catch (std::exception& e) {
    SDL_LogWarn(0, "Error trying to set the field of view: %s", e.what());
  }
  cam.setAspectRatio((float)width / height);

  // Initially there are zero balls in the system
  balls = NULL;
  ballCount = 0;

  loadGeometry();
  initShaders();
}

/**
 * The destructor frees the memory reserved for storing the balls in the system
 * and the shaders.
 */
Scene3D::~Scene3D() {
  // Delete balls from memory
  clearBalls();
  // Remove shaders from memory
  glDeleteShader(vertexShader);
  glDeleteShader(pixelShader);
  glDeleteProgram(shaderProgram);
}

/**
 * Creates the SDL window with the given title and size.
 */
void Scene3D::initWindow(char const* titleStr, int w, int h) {
  // Use an empty string if title is not given.
  if (titleStr == NULL) titleStr = "";

// Set width and height to the browser's size when running in browser
#ifdef __EMSCRIPTEN__
  w = WidthGetter::getBrowserWidth();
  h = WidthGetter::getBrowserHeight();
#endif
  width = w;
  height = h;
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  // Create SDL window
  window = SDL_CreateWindow(
      titleStr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

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

/**
 * Loads the geometry into memory. Generates the sphere that will be used when
 * rendering the balls and loads the base scene from the scene file.
 */
void Scene3D::loadGeometry() {
  // Generate the ball model to be used and load it into GPU memory
  content.loadModel();
  content.loadToGL();

  std::ifstream file;
  file.exceptions(std::ifstream::badbit);
  try {
    // Load the file with the overloaded operator
    file.open("base.scene");
    file >> (*this);
  } catch (const std::ifstream::failure& e) {
    SDL_Log("Could not open and read file: base.scene");
  }
  file.close();

  // Load the scene geometry into GPU memory
  world.loadToGL();
}

/**
 * Loads shaders and links them into a program.
 */
void Scene3D::initShaders() {
  // Load vertex shader
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &Shaders::baseVertex, NULL);
  glCompileShader(vertexShader);
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    SDL_LogError(0, "An error occured while compiling the vertex shader: %s",
                 infoLog);
  }

  // Load pixel shader
  pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(pixelShader, 1, &Shaders::customColorFragment, NULL);
  glCompileShader(pixelShader);
  glGetShaderiv(pixelShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(pixelShader, 512, NULL, infoLog);
    SDL_LogError(0, "An error occured while compiling the pixel shader: %s",
                 infoLog);
  }

  // Link the shaders into a program and tell
  // OpenGL to use it
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, pixelShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    SDL_LogError(0,
                 "An error occured while constructing the shader program: %s",
                 infoLog);
  }
  glUseProgram(shaderProgram);

  // Get locations in shaders
  posAttrib = glGetAttribLocation(shaderProgram, "vertexPos");
  projectionLocation = glGetUniformLocation(shaderProgram, "proj");
  modelViewLocation = glGetUniformLocation(shaderProgram, "modelView");
  colorLocation = glGetUniformLocation(shaderProgram, "color");

  // Tell OpenGL the format of the vertices
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(posAttrib);

  // Enable Z-buffering and Z-buffer fill offset for better wireframe visibility
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
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

/**
 * Calling this funcion makes the app enter the main loop.
 */
void Scene3D::enterLoop() {
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
  while (isRunning) {
    loopFunc();
  }
#endif
}

void Scene3D::mainLoop(Uint32 t) {
  // Set background color (black)
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Move camera when WASD or space or shift is pressed
  if (WASDKeys[1]) cam.moveBy(-cam.getRightDir() * 1.8f);
  if (WASDKeys[3]) cam.moveBy(cam.getRightDir() * 1.8f);
  if (WASDKeys[0]) cam.moveBy(cam.getForwardDir() * 1.8f);
  if (WASDKeys[2]) cam.moveBy(-cam.getForwardDir() * 1.8f);
  if (spaceKey) cam.moveBy(Vec3(0.0f, 1.8f, 0.0f));
  if (shiftKey) cam.moveBy(Vec3(0.0f, -1.8f, 0.0f));

  // Update the balls if time is not frozen
  if (!timeStopped) {
    Vec3 gravity = Vec3(0, -200, 0);
    for (int i = 0; i < ballCount; i++) {
      balls[i].update(1.0f / 60.0f, gravity);
      // Ball-ball collisions
      for (int j = i + 1; j < ballCount; j++) Ball::collide(balls[i], balls[j]);
      // Ball-world collisions
      balls[i].collideWithModel(world);
    }
  }

  // Set matrices
  Matrix modelViewMatrix;
  Matrix projMatrix = cam.getProjMatrix(0.75f, 2000.0f);
  glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projMatrix.getElements());
  glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE,
                     modelViewMatrix.getElements());

  // Render the world
  // Normal rendering
  glUniform4f(colorLocation, 0.75f, 0.75f, 0.75f, 1.0f);
  world.render(posAttrib);
  // Wireframe rendering
  glUniform4f(colorLocation, 0.05f, 0.05f, 0.2f, 1.0f);
  world.renderOneByOne(posAttrib, GL_LINE_LOOP);

  // Render the balls
  for (int i = 0; i < ballCount; i++) {
    // Set the transform of the ball
    modelViewMatrix = balls[i].getModelViewMatrix();
    glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE,
                       modelViewMatrix.getElements());

    // Render the sphere looking like a beach ball
    glUniform4f(colorLocation, 0.75f, 0.12f, 0.12f, 1.0f);
    content.renderStriped(posAttrib);
    glUniform4f(colorLocation, 0.8f, 0.8f, 0.8f, 1.0f);
    content.renderStriped(posAttrib, true);
  }
}

/**
 * Handles keyboard button down events.
 */
void Scene3D::keyDownEvent(const SDL_KeyboardEvent& e) {
  switch (e.keysym.sym) {
    // Set the correspontding boolean values true
    case SDLK_w:
      WASDKeys[0] = true;
      break;
    case SDLK_a:
      WASDKeys[1] = true;
      break;
    case SDLK_s:
      WASDKeys[2] = true;
      break;
    case SDLK_d:
      WASDKeys[3] = true;
      break;
    case SDLK_SPACE:
      spaceKey = true;
      break;
    case SDLK_LSHIFT:
      shiftKey = true;
      break;
    case SDLK_r:
      // Clears the ball array when pressing R
      clearBalls();
      break;
    default:
      break;
  }
}

/**
 * Handles keyboard button release events.
 */
void Scene3D::keyUpEvent(const SDL_KeyboardEvent& e) {
  switch (e.keysym.sym) {
    case SDLK_ESCAPE:
      SDL_SetRelativeMouseMode(SDL_FALSE);
      break;
      // Set the correspontding boolean values false
    case SDLK_w:
      WASDKeys[0] = false;
      break;
    case SDLK_a:
      WASDKeys[1] = false;
      break;
    case SDLK_s:
      WASDKeys[2] = false;
      break;
    case SDLK_d:
      WASDKeys[3] = false;
      break;
    case SDLK_SPACE:
      spaceKey = false;
      break;
    case SDLK_LSHIFT:
      shiftKey = false;
      break;
    case SDLK_t:
      // Stop the time when pressing T
      timeStopped = !timeStopped;
      break;
    case SDLK_p:
      // Save the scene into a file when pressing P
      saveScene("saved.scene");
      break;
    default:
      break;
  }
}

/**
 * Handles mouse motion, rotates the camera accordingly.
 */
void Scene3D::mouseMotionEvent(const SDL_MouseMotionEvent& e) {
  cam.tiltUp(-(float)e.yrel / 1500.0f);
  cam.turnRight((float)e.xrel / 1500.0f);
}

/**
 * Handles the mouse button press event, places a ball on the left mouse button
 * press.
 */
void Scene3D::mouseButtonDownEvent(const SDL_MouseButtonEvent& e) {
  if (e.button == SDL_BUTTON_LEFT) {
    if (!SDL_GetRelativeMouseMode())
      SDL_SetRelativeMouseMode(SDL_TRUE);
    else
      placeBall();
  }
}

/**
 * Handles the window resize event.
 */
void Scene3D::resizeEvent(int width, int height) {
  // If running in the browser, get the new width and height from JS
#ifdef __EMSCRIPTEN__
  int w, h;
  Scene3D::getBrowserDimensions(&w, &h);
#else
  int w = width;
  int h = height;
#endif
  // Set the new render target size and the new aspect ratio of the camera
  glViewport(0, 0, w, h);
  cam.setAspectRatio((float)w / h);
}

/**
 * Handles the file drag and drop event.
 */
void Scene3D::fileDropEvent(const char* fName) {
  SDL_Log("Loading file %s", fName);
  std::ifstream file;
  file.exceptions(std::ifstream::badbit);
  try {
    // Load the file containing a scene with the overloaded operator
    file.open(fName);
    file >> (*this);
  } catch (const std::ifstream::failure& e) {
    SDL_Log("Could not open and read file: %s", fName);
  }
  file.close();

  // Load the newly read geometry to GPU memory
  world.loadToGL();
}

/**
 * Shoots a ball out of the camera.
 */
void Scene3D::placeBall() {
  Ball* newAddr = new Ball[ballCount + 1];
  std::memcpy(newAddr, balls, sizeof(Ball) * ballCount);
  ballCount++;
  if (balls != NULL) delete[] balls;
  balls = newAddr;
  // Set the default settings for the new ball
  balls[ballCount - 1].setPosition(cam.getPos());
  balls[ballCount - 1].setVel(cam.getDir());
  balls[ballCount - 1].setRadius(5);
  balls[ballCount - 1].setBounciness(0.15f);
  balls[ballCount - 1].setFrictionCoefficient(0.3f);
}

/**
 * Appends a ball to the current ones present.
 */
void Scene3D::addBall(const Ball& b) {
  Ball* newAddr = new Ball[ballCount + 1];
  std::memcpy(newAddr, balls, sizeof(Ball) * ballCount);
  ballCount++;
  if (balls != NULL) delete[] balls;
  balls = newAddr;
  balls[ballCount - 1] = b;
}

/**
 * Removes the balls from the scene and frees up allocated memory.
 */
void Scene3D::clearBalls() {
  if (balls != NULL) delete[] balls;
  balls = NULL;
  ballCount = 0;
}

/**
 * This function saves the state of the scene into an obj file (the plan is to
 * save the balls' positions and other attributes in obj comments). It only
 * stores the position and size of balls.
 */
std::ostream& operator<<(std::ostream& os, const Scene3D& scene) {
  // Store balls
  for (int i = 0; i < scene.ballCount; i++) {
    Ball& b = scene.balls[i];
    Vec3 pos = b.getPosition();
    os << "#ball " << pos.x << ' ' << pos.y << ' ' << pos.z << ' '
       << b.getRadius() << ' ' << b.getDensity() << ' '
       << b.getAngularMassMultiplier() << ' ' << b.getBounciness() << ' '
       << b.getFrictionCoefficient() << '\n';
  }
  // Indicate that there are no more balls to be read
  os << "#end\n";

  // Store vertices
  auto vNum = scene.world.getVertexNum();
  for (int i = 0; i < vNum; i++) {
    auto v = scene.world.getVertex(i);
    os << "v " << v.x << ' ' << v.y << ' ' << v.z << '\n';
  }

  // Store the triangle indices
  auto tNum = scene.world.getTriangleNum();
  for (int i = 0; i < tNum; i++) {
    GLuint a, b, c;
    scene.world.getTriangleIdx(i, &a, &b, &c);
    // Important that obj indexing starts from 1
    os << "f " << (a + 1) << ' ' << (b + 1) << ' ' << (c + 1) << '\n';
  }

  return os;
}

/**
 * Loads a scene from a file.
 */
std::istream& operator>>(std::istream& is, Scene3D& scene) {
  std::string line;
  scene.clearBalls();
  // Load the balls in the scene described in the starting lines of the file
  while (std::getline(is, line)) {
    // If the line describes a ball, load it
    if ((line.length() >= 6) && line[0] == '#' && line[1] == 'b' &&
        line[2] == 'a' && line[3] == 'l' && line[4] == 'l' && line[5] == ' ') {
      std::istringstream loader(line.substr(6));
      float x, y, z, r, density, angularMMult, bounciness, fc;
      loader >> x >> y >> z >> r >> density >> angularMMult >> bounciness >> fc;
      Ball newBall(Vec3(x, y, z), r);
      // Set the attributes of the newly loaded ball
      newBall.setDensity(density);
      newBall.setAngularMassMultiplier(angularMMult);
      newBall.setBounciness(bounciness);
      newBall.setFrictionCoefficient(fc);
      scene.addBall(newBall);
    } else
      break;
  }

  // Then load the rest of the file as a basic obj
  is >> scene.world;
  // Load the scene into GPU memory
  // A good thing is that OpenGL deletes the old geometry data if this is not
  // the first scene loaded
  scene.world.loadToGL();

  return is;
}

/**
 * Saves the scene with the given file name.
 */
void Scene3D::saveScene(const char* fileName) const {
  std::ofstream saveFile(fileName);
  if (saveFile.is_open()) {
    saveFile << (*this);
    saveFile.close();
  }
}

/**
 * This function handles the incoming events and calls the right virtual
 * function for the given event.
 */
void Scene3D::handleEvents() {
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
      case SDL_DROPFILE:
        fileDropEvent(e.drop.file);
        // Free up the memory of the file name string
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
