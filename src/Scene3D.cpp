#include "Scene3D.h"

Scene3D::Scene3D(char const* titleStr) {
  WASDKeys[0] = WASDKeys[1] = WASDKeys[2] = WASDKeys[3] = spaceKey = shiftKey =
      false;
  initWindow(titleStr);
  this->loadGeometry();
  this->initShaders();

  cam.setFOV(M_PI / 2.0f);
  cam.setAspectRatio(1.0f);

  if (!SDL_GetRelativeMouseMode()) SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Scene3D::loadGeometry() {
  // Init vertex array
  GLuint vertexArr;
  glGenVertexArraysOES(1, &vertexArr);
  glBindVertexArrayOES(vertexArr);

  // Load an octahedron
  // The first array contains the vertices of the object
  GLfloat vs[] = {0.0f,  1.0f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f,  -1.0f,
                  -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,  -1.0f, 0.0f};
  // The second array contains the index triplets describing the triangles to
  // render The concept is pretty similar to how obj files store 3D models
  GLushort is[] = {4, 0, 1, 1, 2, 0, 2, 3, 0, 3, 4, 0,
                   4, 1, 5, 5, 1, 2, 2, 3, 5, 3, 4, 5};
  indices = is;
  cubeVertices = vs;

  // Load the vertices into the vertex buffer
  GLuint vertexBufferObj;
  glGenBuffers(1, &vertexBufferObj);
  // Load the vertex indices into the element buffer
  GLuint elementBufferObj;
  glGenBuffers(1, &elementBufferObj);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vs), cubeVertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObj);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(is), indices, GL_STATIC_DRAW);
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
  glEnableVertexAttribArray(posAttrib);

  // Enable Z-buffering
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

void Scene3D::mainLoop(Uint32 t) {
  // Set background color (black)
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Rotate projection matrix when A or D is pressed
  if (WASDKeys[1]) cam.moveBy(cam.getRightDir() * -0.1f);
  if (WASDKeys[3]) cam.moveBy(cam.getRightDir() * 0.1f);
  if (WASDKeys[0]) cam.moveBy(cam.getForwardDir() * 0.1f);
  if (WASDKeys[2]) cam.moveBy(cam.getForwardDir() * -0.1f);
  if (spaceKey) cam.moveBy(Vec3(0.0f, 0.1f, 0.0f));
  if (shiftKey) cam.moveBy(Vec3(0.0f, -0.1f, 0.0f));

  Matrix modelViewMatrix = Matrix::translation(0.0f, 0.0f, -10.0f);

  // Set matrices
  Matrix projMatrix = cam.getProjMatrix(0.1f, 100.0f);
  glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &(projMatrix.m[0]));
  glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, &(modelViewMatrix.m[0]));

  // Render the object in grey
  glUniform4f(colorLocation, 0.75f, 0.75f, 0.75f, 0.75f);
  glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_SHORT, 0);
  // And also render it's wireframe in dark blue
  // I'm not planning on implementing shading so I use wireframe rendering so
  // the user can get a sense of the object's shape
  glUniform4f(colorLocation, 0.0f, 0.0f, 0.125f, 1.0f);
  glDrawElements(GL_LINE_LOOP, 24, GL_UNSIGNED_SHORT, 0);
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
    default:
      break;
  }
}

void Scene3D::mouseMotionEvent(const SDL_MouseMotionEvent& e) {
  cam.tiltUp(-(float)e.yrel / 1500.0f);
  cam.turnRight((float)e.xrel / 1500.0f);
}
