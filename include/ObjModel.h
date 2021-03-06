/**
 * ©·2021·Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

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
  friend std::istream& operator>>(std::istream& is, ObjModel&);
};

#endif