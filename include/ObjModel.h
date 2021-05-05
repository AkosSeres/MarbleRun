#ifndef _PHY3D_OBJ_MODEL_H_
#define _PHY3D_OBJ_MODEL_H_

#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#include "Model.h"

class ObjModel : public Model {
 protected:
  std::string fileName;

 public:
  ObjModel(const char* fileName_ = "") : Model(), fileName(fileName_){};
  void setFileName(const char* name) { fileName = name; };
  void loadModel() override;
};

#endif