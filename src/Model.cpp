#include "Model.h"

/**
 * Initalises an empty model.
 */
Model::Model() {
  vertices = NULL;
  vCount = 0;
  indices = NULL;
  iCount = 0;

  // Create vertex and index buffers
  glGenBuffers(1, &vertexBufferObj);
  glGenBuffers(1, &elementBufferObj);

  loadModel();
}

/**
 * Frees the loaded memory.
 */
Model::~Model() {
  delete[] vertices;
  delete[] indices;
}

/**
 * Loads vertices and corresponding vertices into memory. In base case it just
 * loads an octahedron, this function is to be overridden by a child class.
 */
void Model::loadModel() {
  // Load a base octahedron
  // The first array contains the vertices of the object
  GLfloat vs[] = {0.0f,  1.0f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f,  -1.0f,
                  -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,  -1.0f, 0.0f};
  // The second array contains the index triplets describing the triangles to
  // render. The concept is pretty similar to how obj files store 3D models.
  GLuint is[] = {4, 0, 1, 1, 2, 0, 2, 3, 0, 3, 4, 0,
                 4, 1, 5, 5, 1, 2, 2, 3, 5, 3, 4, 5};

  vertices = vs;
  vCount = 18;
  indices = is;
  iCount = 24;
}

/**
 * Loads the vertex and index data into the OpenGL buffers.
 */
void Model::loadToGL() const {
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vCount, vertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObj);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iCount, indices,
               GL_STATIC_DRAW);
}

/**
 * Binds the corresponding OGL buffers for custom rendering.
 */
void Model::bindBuffers() const {
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObj);
}

/**
 * Renders the model with the given rendering mode.
 */
void Model::render(GLenum mode) const {
  bindBuffers();
  glDrawElements(mode, iCount, GL_UNSIGNED_INT, 0);
}

/**
 * Constructor of the sphere model. Sets the resolution and radius of the sphere
 * to be generated.
 */
SphereModel::SphereModel(int resolution, float radius) {
  vertices = NULL;
  vCount = 0;
  indices = NULL;
  iCount = 0;

  // Create vertex and index buffers
  glGenBuffers(1, &vertexBufferObj);
  glGenBuffers(1, &elementBufferObj);

  sphereResolution = resolution;
  R = radius;
}

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
void SphereModel::renderStriped(bool side) const {
  bindBuffers();
  GLuint triCount = iCount / 3;
  for (int i = 0; i < triCount; i += 4) {
    if (side)
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((i + 2) * 12));
    else
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 12));
  }
}
