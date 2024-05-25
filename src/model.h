#ifndef __MODEL_H__
#define __MODEL_H__

#include "geometry.h"
#include <vector>

class Model {
private:
  std::vector<Vec3f> verts_;
  std::vector<Vec2f> uvs_;
  std::vector<Vec3f> normals_;
  std::vector<std::vector<int>> faces_;

public:
  Model(const char *filename);
  ~Model();
  int nverts();
  int nuvs();
  int nnormals();
  int nfaces();
  Vec3f vert(int i);
  std::vector<int> face(int idx);
};

#endif //__MODEL_H__
