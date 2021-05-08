/**
 * ©·2021·Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include "Model.h"

/**
 * Initalises an empty model.
 */
Model::Model() {
  // Initalise vertex and index array pointers as NULL
  vertices = NULL;
  vCount = 0;
  indices = NULL;
  iCount = 0;

  // Create vertex and index buffers for OpenGL
  glGenBuffers(1, &vertexBufferObj);
  glGenBuffers(1, &elementBufferObj);
}

/**
 * Frees the loaded memory.
 */
Model::~Model() {
  // Remove model data from CPU memory
  delete[] vertices;
  delete[] indices;
  // And remove the same data from GPU memory
  glDeleteBuffers(1, &vertexBufferObj);
  glDeleteBuffers(1, &elementBufferObj);
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
 * Loads the vertex and index data into the OpenGL buffers. If some data is
 * already loaded, OpenGL automatically removes it, and replaces it with the new
 * data.
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
void Model::render(GLint posAttrib, GLenum mode) const {
  // Bind buffers to use them while rendering
  bindBuffers();
  // Set the vertex attrib pointer
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
  // Render the model in the given mode
  glDrawElements(mode, iCount, GL_UNSIGNED_INT, 0);
}

/**
 * Renders the model with the given rendering mode one triangle at a time. Can
 * be used for wireframe rendering.
 */
void Model::renderOneByOne(GLint posAttrib, GLenum mode) const {
  bindBuffers();
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
  // Runs through the triangles in a for loop rendering them one by one
  for (GLuint i = 0; i < (iCount / 3); i++)
    glDrawElements(mode, 3, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * i * 3));
}

/**
 * Returns the number of vertices in the model.
 */
GLuint Model::getVertexNum() const { return vCount / 3; }

/**
 * Returns the vertex at the given index as a Vec3.
 */
Vec3 Model::getVertex(GLuint index) const {
  return Vec3(vertices[index * 3], vertices[index * 3 + 1],
              vertices[index * 3 + 2]);
}

/**
 * Returns the number of triangles in the model.
 */
GLuint Model::getTriangleNum() const { return iCount / 3; }

/**
 * Loads the requested triangle's vertices into the Vec3 pointers.
 */
void Model::getTriangle(GLuint index, Vec3* a, Vec3* b, Vec3* c) const {
  (*a) = getVertex(indices[index * 3]);
  (*b) = getVertex(indices[index * 3 + 1]);
  (*c) = getVertex(indices[index * 3 + 2]);
}

/**
 * Loads the requested triangle's vertex indices into the pointers.
 */
void Model::getTriangleIdx(GLuint index, GLuint* a, GLuint* b,
                           GLuint* c) const {
  (*a) = (indices[index * 3]);
  (*b) = (indices[index * 3 + 1]);
  (*c) = (indices[index * 3 + 2]);
}
