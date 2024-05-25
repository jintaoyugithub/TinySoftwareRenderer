#ifndef __MODEL_H__
#define __MODEL_H__

#include "geometry.h"
#include <vector>

struct face {
  std::vector<int> vertsIdx;
  std::vector<int> uvsIdx;
  std::vector<int> normalsIdx;
};

class Model {
private:
  std::vector<Vec3f> verts_;
  std::vector<Vec2f> uvs_;
  std::vector<Vec3f> normals_;
  std::vector<face> faces_;

public:
  Model(const char *filename);
  ~Model();
  int nverts();
  int nuvs();
  int nnormals();
  int nfaces();
  Vec3f vert(int i) const;
  Vec2f uv(int i) const;
  Vec3f norm(int i) const;
  face face(int idx);
};

#endif //__MODEL_H__
