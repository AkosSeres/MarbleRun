#include "ObjModel.h"

/**
 * Loads the geometry from the given file.
 */
void ObjModel::loadModel() {
  std::ifstream file;
  file.exceptions(std::ifstream::badbit);

  try {
    file.open(fileName);
    file >> (*this);
  } catch (const std::ifstream::failure& e) {
    SDL_Log("Could not open and read file: %s", fileName.c_str());
  }

  file.close();
}

/**
 * Reads the obj file from an input stream.
 */
std::istream& operator>>(std::istream& is, ObjModel& model) {
  auto& vertices = model.vertices;
  auto& indices = model.indices;
  delete[] vertices;
  delete[] indices;
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

  while (getline(is, line)) {
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

  model.vCount = vertNum * 3;
  model.iCount = triangleNum * 3;

  return is;
}