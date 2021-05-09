/**
 * ©·2021·Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include "ObjModel.h"

/**
 * Loads the geometry from the given file with the stored file name.
 */
void ObjModel::loadModel() {
  std::ifstream file;
  file.exceptions(std::ifstream::badbit);

  try {
    // Open file and read the data from the file stream
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
  // Free up the memory first
  if (vertices != NULL) delete[] vertices;
  if (indices != NULL) delete[] indices;
  std::string line;
  // Initially allocate memory for 512 vertices and triangles
  GLuint vAlloc = 512;
  GLuint iAlloc = 512;
  GLuint vertNum = 0;      // The number of vertices read from the input stream
  GLuint triangleNum = 0;  // The number of triangles read from the input stream
  // Allocate the initial memory space
  vertices = new GLfloat[vAlloc * 3];
  indices = new GLuint[iAlloc * 3];

  // Lambda function for allocating more (twice as much) space for loading more
  // vertices
  auto reallocVertices = [&]() {
    // Allocate new space
    GLfloat* newAddr = new GLfloat[vAlloc * 6];
    // Copy old data
    std::memcpy(newAddr, vertices, sizeof(GLfloat) * 3 * vAlloc);
    // Free up old data
    delete[] vertices;
    vertices = newAddr;
    vAlloc *= 2;
  };

  // Lambda fucntion to append a new vertex to the list and allocate more space
  // automatically if needed
  auto appendVertex = [&](float x, float y, float z) {
    vertices[3 * vertNum] = x;
    vertices[3 * vertNum + 1] = y;
    vertices[3 * vertNum + 2] = z;
    vertNum++;
    if (vertNum > (vAlloc - 10)) reallocVertices();
  };

  // Lambda function for allocating more (twice as much) space for loading more
  // indices
  auto reallocIndices = [&]() {
    GLuint* newAddr = new GLuint[iAlloc * 6];
    std::memcpy(newAddr, indices, sizeof(GLuint) * 3 * iAlloc);
    delete[] indices;
    indices = newAddr;
    iAlloc *= 2;
  };

  // Lambda fucntion to append a new index triplet to the list of triangle
  // indices and it allocates more space automatically if needed
  auto appendIndices = [&](GLuint a, GLuint b, GLuint c) {
    indices[3 * triangleNum] = a;
    indices[3 * triangleNum + 1] = b;
    indices[3 * triangleNum + 2] = c;
    triangleNum++;
    if (triangleNum > (iAlloc - 10)) reallocIndices();
  };

  // Lambda function for extracting an integer from a string
  auto extractNumber = [](std::string& s) {
    std::istringstream ss(s);
    int num;
    ss >> num;
    return num;
  };

  // Load all the lines in order
  // Treats the file as a Wavefront obj file, but it does not deal with all
  // types of it, so it is best to use the program only with the given scene
  // files
  while (getline(is, line)) {
    // The line contains a vertex if it starts with the letter v
    if (line[0] == 'v' && (line[1] == ' ' || line[1] == '\t')) {
      std::istringstream loader(line.substr(1));
      // Load the float coordinates and add them to the list with the lambda
      // function
      float x, y, z;
      loader >> x >> y >> z;
      appendVertex(x, y, z);
    }
    // The currently read line describes a triangle/polygon if it starts with
    // the letter f
    if (line[0] == 'f' && (line[1] == ' ' || line[1] == '\t')) {
      std::istringstream loader(line.substr(1));
      // Load them as strings first then convert them into integers
      std::string i1, i2, i3;
      loader >> i1 >> i2 >> i3;
      appendIndices(extractNumber(i1) - 1, extractNumber(i2) - 1,
                    extractNumber(i3) - 1);
      std::string more;
      loader >> more;
      // If there are more indices in the given line (can be anything more than
      // 3), load them too
      while (more.length() > 0) {
        i1 = i2;
        i2 = i3;
        i3 = more;
        appendIndices(extractNumber(i1) - 1, extractNumber(i2) - 1,
                      extractNumber(i3) - 1);
        loader >> more;
      }
    }
  }

  // Store the length of the arrays containing the vertices and triangles
  model.vCount = vertNum * 3;
  model.iCount = triangleNum * 3;

  // Return the input stream
  return is;
}