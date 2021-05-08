#include "Scene3D.h"

Scene3D::Scene3D(char const* titleStr) {
  WASDKeys[0] = WASDKeys[1] = WASDKeys[2] = WASDKeys[3] = spaceKey = shiftKey =
      timeStopped = false;
  initWindow(titleStr);
  this->loadGeometry();
  this->initShaders();

  cam.setFOV((M_PI / 2.0f));
  cam.setAspectRatio((float)width / height);

  balls = NULL;
  ballCount = 0;

  if (!SDL_GetRelativeMouseMode()) SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Scene3D::loadGeometry() {
  content.setRadius(1.0f);
  content.setResolution(18);
  content.loadModel();
  content.loadToGL();

  std::ifstream file;
  file.exceptions(std::ifstream::badbit);
  try {
    file.open("base.scene");
    file >> (*this);
  } catch (const std::ifstream::failure& e) {
    SDL_Log("Could not open and read file: base.scene");
  }
  file.close();

  world.loadToGL();
}

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
  // glEnableVertexAttribArray(posAttrib);

  // Enable Z-buffering
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

void Scene3D::mainLoop(Uint32 t) {
  // Set background color (black)
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Rotate projection matrix when A or D is pressed
  if (WASDKeys[1]) cam.moveBy(-cam.getRightDir());
  if (WASDKeys[3]) cam.moveBy(cam.getRightDir());
  if (WASDKeys[0]) cam.moveBy(cam.getForwardDir());
  if (WASDKeys[2]) cam.moveBy(-cam.getForwardDir());
  if (spaceKey) cam.moveBy(Vec3(0.0f, 1.0f, 0.0f));
  if (shiftKey) cam.moveBy(Vec3(0.0f, -1.0f, 0.0f));

  // Update the balls if time is not frozen
  if (!timeStopped) {
    Vec3 gravity = Vec3(0, -100, 0);
    for (int i = 0; i < ballCount; i++) {
      balls[i].update(1.0f / 60.0f, gravity);
      balls[i].collideWithModel(world);
    }
  }

  // Set matrices
  Matrix modelViewMatrix;
  Matrix projMatrix = cam.getProjMatrix(0.5f, 350.0f);
  glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &(projMatrix.m[0]));
  glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, &(modelViewMatrix.m[0]));

  // Render the world
  glUniform4f(colorLocation, 0.05f, 0.05f, 0.2f, 1.0f);
  world.renderOneByOne(posAttrib, GL_LINE_LOOP);
  glUniform4f(colorLocation, 0.75f, 0.75f, 0.75f, 1.0f);
  world.render(posAttrib);

  // Render the balls
  for (int i = 0; i < ballCount; i++) {
    // Set the transform of the ball
    modelViewMatrix = balls[i].getModelViewMatrix();
    glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, &(modelViewMatrix.m[0]));

    // Render the sphere looking like a beach ball
    glUniform4f(colorLocation, 0.75f, 0.12f, 0.12f, 1.0f);
    content.renderStriped(posAttrib);
    glUniform4f(colorLocation, 0.8f, 0.8f, 0.8f, 1.0f);
    content.renderStriped(posAttrib, true);
  }
}

void Scene3D::keyDownEvent(const SDL_KeyboardEvent& e) {
  switch (e.keysym.sym) {
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
    default:
      break;
  }
}
void Scene3D::keyUpEvent(const SDL_KeyboardEvent& e) {
  switch (e.keysym.sym) {
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
      timeStopped = !timeStopped;
      break;
    case SDLK_p:
      saveScene("saved.scene");
      break;
    default:
      break;
  }
}

void Scene3D::mouseMotionEvent(const SDL_MouseMotionEvent& e) {
  cam.tiltUp(-(float)e.yrel / 1500.0f);
  cam.turnRight((float)e.xrel / 1500.0f);
}

void Scene3D::mouseButtonDownEvent(const SDL_MouseButtonEvent& e) {
  if (e.button == SDL_BUTTON_LEFT) placeBall();
}

void Scene3D::resizeEvent(int width, int height) {
  glViewport(0, 0, width, height);
}

void Scene3D::fileDropEvent(const char* fName) {
  SDL_Log("Loading file %s", fName);
  std::ifstream file;
  file.exceptions(std::ifstream::badbit);
  try {
    file.open(fName);
    file >> (*this);
  } catch (const std::ifstream::failure& e) {
    SDL_Log("Could not open and read file: %s", fName);
  }
  file.close();

  world.loadToGL();
}

/**
 * Shoots a ball out of the camera.
 */
void Scene3D::placeBall() {
  Ball* newAddr = new Ball[ballCount + 1];
  std::memcpy(newAddr, balls, sizeof(Ball) * ballCount);
  ballCount++;
  delete[] balls;
  balls = newAddr;
  balls[ballCount - 1].setPosition(cam.getPos());
  balls[ballCount - 1].setVel(cam.getDir());
  balls[ballCount - 1].setRadius(5);
}

/**
 * Appends a ball to the current ones present.
 */
void Scene3D::addBall(const Ball& b) {
  Ball* newAddr = new Ball[ballCount + 1];
  std::memcpy(newAddr, balls, sizeof(Ball) * ballCount);
  ballCount++;
  delete[] balls;
  balls = newAddr;
  balls[ballCount - 1] = b;
}

Scene3D::~Scene3D() { delete[] balls; }

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
  os << "#end\n";

  // Store vertices
  auto vNum = scene.world.getVertexNum();
  for (int i = 0; i < vNum; i++) {
    auto v = scene.world.getVertex(i);
    os << "v " << v.x << ' ' << v.y << ' ' << v.z << ' ' << "1.0\n";
  }

  // Store the triangle indices
  auto tNum = scene.world.getTriangleNum();
  for (int i = 0; i < tNum; i++) {
    GLuint a, b, c;
    scene.world.getTriangleIdx(i, &a, &b, &c);
    os << "f " << (a + 1) << ' ' << (b + 1) << ' ' << (c + 1) << '\n';
  }

  return os;
}

/**
 * Loads a scene from a file.
 */
std::istream& operator>>(std::istream& is, Scene3D& scene) {
  std::string line;
  delete[] scene.balls;
  scene.ballCount = 0;
  // Load the balls in the scene described in the starting lines of the file
  while (std::getline(is, line)) {
    // If the line describes a ball, load it
    if ((line.length() >= 6) && line[0] == '#' && line[1] == 'b' &&
        line[2] == 'a' && line[3] == 'l' && line[4] == 'l' && line[5] == ' ') {
      std::istringstream loader(line.substr(5));
      float x, y, z, r, density, angularMMult, bounciness, fc;
      loader >> x >> y >> z >> density >> angularMMult >> bounciness >> fc;
      Ball newBall(Vec3(x, y, z), r);
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