#ifndef _PHY3D_MODEL_H_
#define _PHY3D_MODEL_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#include "Vec3.h"

class Model {
 protected:
  GLfloat* vertices;
  GLuint vCount;
  GLuint* indices;
  GLuint iCount;

  GLuint vertexBufferObj;
  GLuint elementBufferObj;

 public:
  Model();
  ~Model();
  virtual void loadModel();
  void loadToGL() const;
  void bindBuffers() const;
  void render(GLenum mode = GL_TRIANGLES) const;
  void renderOneByOne(GLenum mode = GL_TRIANGLES) const;
};

class SphereModel : public Model {
 protected:
  int sphereResolution;
  float R;

 public:
  SphereModel(int resolution = 12, float radius = 1.0f);
  ~SphereModel();
  void setResolution(int newRes) { sphereResolution = newRes; }
  void setRadius(float r) { R = r; }
  void loadModel() override;
  void renderStriped(bool side = false) const;
};

class ObjModel : public Model {
 protected:
  std::string fileName;

 public:
  ObjModel(const char* fileName_ = "");
  ~ObjModel();
  void setFileName(const char* name) { fileName = name; };
  void loadModel() override;
};

#endif