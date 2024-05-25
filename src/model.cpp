#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/* Model::Model(const char *filename) : verts_(), faces_() { */
/*   std::ifstream in; */
/*   in.open(filename, std::ifstream::in); */
/*   if (in.fail()) */
/*     return; */
/*   std::string line; */
/*   while (!in.eof()) { */
/*     std::getline(in, line); */
/*     std::istringstream iss(line.c_str()); */
/*     char trash; */
/*     if (!line.compare(0, 2, "v ")) { */
/*       // put "v " into trash */
/*       iss >> trash; */
/*       Vec3f v; */
/*       for (int i = 0; i < 3; i++) */
/*         iss >> v.raw[i]; */
/*       verts_.push_back(v); */
/*     } else if (!line.compare(0, 3, "vt ")) { // get the uv coordinates */
/*       iss >> trash; */
/*       Vec2f uv; */
/*       for (int i = 0; i < 2; i++) { */
/*         iss >> uv.raw[i]; */
/*       } */
/*       uvs_.push_back(uv); */
/*     } else if (!line.compare(0, 3, "vn ")) { // get the normal */
/*       iss >> trash; */
/*       Vec3f norm; */
/*       for (int i = 0; i < 3; i++) { */
/*         iss >> norm.raw[i]; */
/*       } */
/*       normals_.push_back(norm); */
/*     } else if (!line.compare(0, 2, "f ")) { */
/*       std::vector<int> f; */
/*       int itrash, idx; */
/*       iss >> trash; */
/*       while (iss >> idx >> trash >> itrash >> trash >> itrash) { */
/*         idx--; // in wavefront obj all indices start at 1, not zero */
/*         f.push_back(idx); */
/*       } */
/*       faces_.push_back(f); */
/*     } else if (!line.compare(0, 2, "f ")) { */
/*     } */
/*   } */
/*   std::cerr << "# v# " << verts_.size() << " uv# " << uvs_.size() <<
 * "normals# " */
/*             << normals_.size() << " f# " << faces_.size() << std::endl; */
/* } */

enum code { vertices, uvs, normals, faces };

int prefixToCode(const std::string &prefix) {
  if (prefix == "v")
    return code::vertices;
  if (prefix == "vt")
    return code::uvs;
  if (prefix == "vn")
    return code::normals;
  if (prefix == "f")
    return code::faces;
  return 0;
}

Model::Model(const char *filename) : verts_(), faces_() {
  std::ifstream in;
  in.open(filename, std::ifstream::in);
  if (in.fail())
    return;
  std::string line;
  while (!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());

    std::string prefix;
    // get the first letter of each line
    iss >> prefix;
    int code = prefixToCode(prefix);

    switch (code) {
    case vertices:
      float x, y, z;
      iss >> x >> y >> z;
      verts_.emplace_back(x, y, z);
      break;
    case uvs:
      break;
    case normals:
      break;
    case faces:
      std::vector<int> f;
      char sTrash;
      int idx, uvTrash, nTrash;
      while (iss >> idx >> sTrash >> uvTrash >> sTrash >> nTrash) {
        idx--;
        f.push_back(idx);
      }
      faces_.push_back(f);
      break;
    }
  }
  std::cerr << "# v# " << verts_.size() << " uv# " << uvs_.size() << "normals# "
            << normals_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model() {}

int Model::nverts() { return (int)verts_.size(); }

int Model::nuvs() { return (int)uvs_.size(); }

int Model::nnormals() { return (int)normals_.size(); }

int Model::nfaces() { return (int)faces_.size(); }

std::vector<int> Model::face(int idx) { return faces_[idx]; }

Vec3f Model::vert(int i) { return verts_[i]; }
