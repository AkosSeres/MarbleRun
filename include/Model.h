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

 private:
  GLuint vertexBufferObj;
  GLuint elementBufferObj;

 public:
  Model();
  ~Model();
  virtual void loadModel();
  void loadToGL() const;
  void bindBuffers() const;
  void render(GLint posAttrib, GLenum mode = GL_TRIANGLES) const;
  void renderOneByOne(GLint posAttrib, GLenum mode = GL_TRIANGLES) const;
  GLuint getVertexNum() const;
  Vec3 getVertex(GLuint index) const;
  GLuint getTriangleNum() const;
  void getTriangle(GLuint index, Vec3* a, Vec3* b, Vec3* c) const;
  void getTriangleIdx(GLuint index, GLuint* a, GLuint* b, GLuint* c) const;
};

#endif