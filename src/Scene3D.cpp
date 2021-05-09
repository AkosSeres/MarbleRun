/**
 * ©·2021·Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include "Scene3D.h"

/**
 * Initalises the application.
 * Cerates the window, loads the base scene and the shaders used for rendering.
 */
Scene3D::Scene3D(char const* titleStr) {
  // The variables indicating some button states
  WASDKeys[0] = WASDKeys[1] = WASDKeys[2] = WASDKeys[3] = spaceKey = shiftKey =
      timeStopped = false;

  // Set the field of view of the camera as well as the aspect ratio
  cam.setFOV((M_PI / 1.75f));
  cam.setAspectRatio((float)width / height);

  // Initially there are zero balls in the system
  balls = NULL;
  ballCount = 0;

  // Create window
  initWindow(titleStr);
  this->loadGeometry();
  this->initShaders();
  // Set mouse capturing
  if (!SDL_GetRelativeMouseMode()) SDL_SetRelativeMouseMode(SDL_TRUE);
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
 * Loads the geometry into memory. Generates the sphere that will be used when
 * rendering the balls and loads the base scene from the scene file.
 */
void Scene3D::loadGeometry() {
  // Generate the ball model to be used and load it into GPU memory
  content.setRadius(1.0f);
  content.setResolution(18);
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

  // Load pixel shader
  pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(pixelShader, 1, &Shaders::customColorFragment, NULL);
  glCompileShader(pixelShader);

  // Link the shaders into a program and tell
  // OpenGL to use it
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, pixelShader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  // Get locations in shaders
  posAttrib = glGetAttribLocation(shaderProgram, "vertexPos");
  projectionLocation = glGetUniformLocation(shaderProgram, "proj");
  modelViewLocation = glGetUniformLocation(shaderProgram, "modelView");
  colorLocation = glGetUniformLocation(shaderProgram, "color");

  // Tell OpenGL the format of the vertices
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Enable Z-buffering and Z-buffer fill offset for better wireframe visibility
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
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
  if (e.button == SDL_BUTTON_LEFT) placeBall();
}

/**
 * Handles the window resize event.
 */
void Scene3D::resizeEvent(int width, int height) {
  // If running in the browser, get the new width and height from JS
#ifdef __EMSCRIPTEN__
  int w, h;
  SDLInstance::getBrowserDimensions(&w, &h);
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
      std::istringstream loader(line.substr(5));
      float x, y, z, r, density, angularMMult, bounciness, fc;
      loader >> x >> y >> z >> density >> angularMMult >> bounciness >> fc;
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
