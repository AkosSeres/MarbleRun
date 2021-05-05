#include "SphereModel.h"

/**
 * Generates a shpere model with the set properties.
 */
void SphereModel::loadModel() {
  int resolution = sphereResolution;
  float radius = R;

  if ((resolution % 2) != 0) resolution -= 1;
  Vec3 up(0, 1, 0);
  Vec3 down(0, -1, 0);
  vCount = (2 + (resolution - 2) * resolution) * 3;
  vertices = new GLfloat[vCount];
  GLuint appended = 0;
  auto appendVertex = [&](const Vec3& v) {
    vertices[3 * appended] = v.x;
    vertices[3 * appended + 1] = v.y;
    vertices[3 * appended + 2] = v.z;
    appended++;
  };
  Vec3* workingVs = new Vec3[resolution - 2];
  for (int i = 0; i < resolution - 2; i++) {
    float upAngle = (float)M_PI / resolution * (1 + i);
    Vec3 newV = up;
    newV.rotateZ(-upAngle).mult(R);
    for (int j = 0; j < resolution; j++) {
      newV.rotateY((float)M_PI * 2 / resolution);
      appendVertex(newV);
    }
  }
  appendVertex(up * R);
  appendVertex(down * R);

  GLuint triangleCount = 2 * resolution * (resolution - 2);
  iCount = triangleCount * 3;
  indices = new GLuint[iCount];
  GLuint appendedIdx = 0;
  auto appendIndex = [&](GLuint a, GLuint b, GLuint c) {
    indices[3 * appendedIdx] = a;
    indices[3 * appendedIdx + 1] = b;
    indices[3 * appendedIdx + 2] = c;
    appendedIdx++;
  };

  for (int i = 0; i < resolution - 3; i++) {
    GLuint offset = i * resolution;
    for (int j = 0; j < resolution; j++) {
      int a_ = offset + j;
      int b_ = offset + ((j + 1) % resolution);
      int c_ = resolution + offset + j;
      int d_ = resolution + offset + ((j + 1) % resolution);
      appendIndex(a_, c_, b_);
      appendIndex(c_, d_, b_);
    }
  }

  GLuint downIdx = appended - 1;
  GLuint upIdx = appended - 2;
  GLuint upOffset = 0;
  GLuint downOffset = (resolution - 3) * resolution;
  for (GLuint i = 0; i < resolution; i++) {
    appendIndex(upOffset + i, upOffset + ((i + 1) % resolution), upIdx);
    appendIndex(downOffset + i, downOffset + ((i + 1) % resolution), downIdx);
  }
}

/**
 * Renders only half of the sphere in stripes.
 */
void SphereModel::renderStriped(GLint posAttrib, bool side) const {
  bindBuffers();
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
  GLuint triCount = iCount / 3;
  for (int i = 0; i < triCount; i += 4) {
    if (side)
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                     (void*)(sizeof(GLuint) * (i + 2) * 3));
    else
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                     (void*)(sizeof(GLuint) * i * 3));
  }
}
