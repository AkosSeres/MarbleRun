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
void Model::render(GLint posAttrib, GLenum mode) const {
  bindBuffers();
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glDrawElements(mode, iCount, GL_UNSIGNED_INT, 0);
}

/**
 * Renders the model with the given rendering mode.
 */
void Model::renderOneByOne(GLint posAttrib, GLenum mode) const {
  bindBuffers();
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
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
 * Frees up the used space.
 */
SphereModel::~SphereModel() {
  delete[] vertices;
  delete[] indices;
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

/**
 * Creates the empty model object for the obj to be loaded.
 */
ObjModel::ObjModel(const char* fileName_) {
  fileName = fileName_;

  vertices = NULL;
  vCount = 0;
  indices = NULL;
  iCount = 0;

  // Create vertex and index buffers
  glGenBuffers(1, &vertexBufferObj);
  glGenBuffers(1, &elementBufferObj);
}

/**
 * Frees up the allocated memory.
 */
ObjModel::~ObjModel() {
  delete[] indices;
  delete[] vertices;
}

/**
 * Loads the geometry from the given file.
 */
void ObjModel::loadModel() {
  std::ifstream file;
  file.exceptions(std::ifstream::badbit);

  try {
    file.open(fileName);
    std::string line;
    GLuint vAlloc = 512;
    GLuint iAlloc = 512;
    GLuint vertNum = 0;
    GLuint triangleNum = 0;
    vertices = new GLfloat[vAlloc * 3];
    indices = new GLuint[iAlloc * 3];

    auto reallocVertices = [&]() {
      GLfloat* newAddr = new GLfloat[vAlloc * 6];
      std::memcpy(newAddr, vertices, sizeof(GLfloat) * 3 * vAlloc);
      delete[] vertices;
      vertices = newAddr;
      vAlloc *= 2;
    };

    auto appendVertex = [&](float x, float y, float z) {
      vertices[3 * vertNum] = x;
      vertices[3 * vertNum + 1] = y;
      vertices[3 * vertNum + 2] = z;
      vertNum++;
      if (vertNum > (vAlloc - 10)) reallocVertices();
    };

    auto reallocIndices = [&]() {
      GLuint* newAddr = new GLuint[iAlloc * 6];
      std::memcpy(newAddr, indices, sizeof(GLuint) * 3 * iAlloc);
      delete[] indices;
      indices = newAddr;
      iAlloc *= 2;
    };

    auto appendIndices = [&](GLuint a, GLuint b, GLuint c) {
      indices[3 * triangleNum] = a;
      indices[3 * triangleNum + 1] = b;
      indices[3 * triangleNum + 2] = c;
      triangleNum++;
      if (triangleNum > (iAlloc - 10)) reallocIndices();
    };

    auto extractNumber = [](std::string& s) {
      std::istringstream ss(s);
      int num;
      ss >> num;
      return num;
    };

    while (getline(file, line)) {
      // The line contains a vertex
      if (line[0] == 'v' && (line[1] == ' ' || line[1] == '\t')) {
        std::istringstream loader(line.substr(1));
        float x, y, z;
        loader >> x >> y >> z;
        appendVertex(x, y, z);
      }
      // The currently read line describes a triangle/polygon
      if (line[0] == 'f' && (line[1] == ' ' || line[1] == '\t')) {
        std::istringstream loader(line.substr(1));
        std::string i1, i2, i3;
        loader >> i1 >> i2 >> i3;
        appendIndices(extractNumber(i1) - 1, extractNumber(i2) - 1,
                      extractNumber(i3) - 1);
        std::string more;
        loader >> more;
        while (more.length() > 0) {
          i2 = i3;
          i3 = more;
          appendIndices(extractNumber(i1) - 1, extractNumber(i2) - 1,
                        extractNumber(i3) - 1);
          loader >> more;
        }
      }
    }

    vCount = vertNum * 3;
    iCount = triangleNum * 3;
  } catch (const std::ifstream::failure& e) {
    SDL_Log("Could not open and read file: %s", fileName.c_str());
  }

  file.close();
}