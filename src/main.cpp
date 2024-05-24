#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

/* constants declaration */
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const Vec3f lightDir = Vec3f(0, 0, -1);
// -- draw mode
enum drawMode { wireframe, filledTri };
// -- size of the image
const int width = 800;
const int height = 800;

/* variables declaration */
Model *model = nullptr;
// zBuffer
float *zBuffer = new float[width * height];

/* functions declaration */
// -- draw line func
void Line(Vec2i, Vec2i, TGAImage &, const TGAColor &);
// -- remap func
int remap(float, Vec2i, Vec2i);
// -- draw triangles func, line sweep
void TriangleLineSweep(const Vec2i *, TGAImage &, const TGAColor &);
// -- draw triangles func, barycentric coordinates
bool isAllGreater0(const int *, int);
bool isAllLess0(const int *, int);
bool isInBarycentric(const Vec2i &, const Vec2f *);
float *barycentricCoords(const Vec2i &, const Vec3f *);
void drawBoundingBox(const Vec2i &, const Vec2i &, TGAImage &,
                     const TGAColor &);
void TriangleBarycentric(const Vec2f *, float *zBuffer, TGAImage &,
                         const TGAColor &);

// -- z buffer
void updateZBuffer(float *zBuffer, int x, int y, float value);
void cleanZBuffer(float *zBuffer);

// -- draw obj models
void drawModel(Model *, float *zBuffer, TGAImage &, int);

int main(int argc, char **argv) {
  TGAImage image(width, height, TGAImage::RGB);
  // load models
  model = new Model("../models/obj/humanHead.obj");
  // init the z buffer
  cleanZBuffer(zBuffer);

  // draw models
  drawModel(model, zBuffer, image, drawMode::filledTri);
  image.flip_vertically();
  image.write_tga_file("output/test.tga");

  delete model;
  delete[] zBuffer;
  return 0;
}

/* functions definition */
void Line(Vec2i start, Vec2i end, TGAImage &img, const TGAColor &col) {
  // determine if it's a steep line
  bool steep = false;
  if (std::abs(start.x - end.x) < std::abs(start.y - end.y)) {
    std::swap(start.x, start.y);
    std::swap(end.x, end.y);
    steep = true;
  }

  // make sure that we will draw the line from left to right
  if (start.x > end.x) {
    std::swap(start.x, end.x);
    std::swap(start.y, end.y);
  }

  // optimization: remove division
  float dx = end.x - start.x;
  float dy = end.y - start.y;
  // wrong: float step = std::abs(dy);
  float step = std::abs(dy) * 2;
  float error = 0;
  float y = start.y;

  if (steep) {
    for (int x = start.x; x < end.x; x++) {
      img.set(y, x, col);

      error += step;
      if (error > dx) {
        // wrong: y += (dy > 0 ? 1 : 0);
        y += (dy > 0 ? 1 : -1);
        error -= 2 * dx;
      }
    }
  } else {
    for (int x = start.x; x < end.x; x++) {
      img.set(x, y, col);

      error += step;
      if (error > dx) {
        // wrong: y += (dy > 0 ? 1 : 0);
        y += (dy > 0 ? 1 : -1);
        error -= 2 * dx;
      }
    }
  }
}

int remap(float var, Vec2i oldRange, Vec2i newRange) {
  return (var - oldRange.x) * (newRange.y - newRange.x) /
         (oldRange.y - oldRange.x);
}

void TriangleLineSweep(std::vector<Vec2i> &vertices, TGAImage &img,
                       const TGAColor &col) {
  // ensure the size of vertices is 3
  if (vertices.size() != 3) {
    std::cout << "wrong vertices number in a single triangle" << std::endl;
    return;
  }

  // make sure 3 vertices are ordered from highest y value to lowest
  if (vertices[0].y > vertices[1].y)
    std::swap(vertices[0], vertices[1]);
  if (vertices[0].y > vertices[2].y)
    std::swap(vertices[0], vertices[2]);
  if (vertices[1].y > vertices[2].y)
    std::swap(vertices[1], vertices[2]);

  // draw the filled tri
  int totalHeight = vertices[2].y - vertices[0].y;
  for (int i = 0; i < totalHeight; i++) {
    bool isSecond =
        i > (vertices[1].y - vertices[0].y) || (vertices[0].y == vertices[1].y);
    // problem here
    float t1 = (float)i / totalHeight;
    // caution: when is in the second half, i should be recalculate to make
    // sure t2 is in 0-1
    float t2 = isSecond ? (float)(i - (vertices[1].y - vertices[0].y)) /
                              (vertices[2].y - vertices[1].y)
                        : (float)i / (vertices[1].y - vertices[0].y);
    // determine the start point of the boundary
    Vec2i boundaryPointLeft = vertices[0] * (1 - t1) + vertices[2] * t1;
    Vec2i boundaryPointRight =
        isSecond ? (vertices[1] * (1 - t2) + vertices[2] * t2)
                 : (vertices[0] * (1 - t2) + vertices[1] * t2);

    // make sure it's render from left to right
    if (boundaryPointLeft.x > boundaryPointRight.x)
      std::swap(boundaryPointLeft, boundaryPointRight);

    for (int j = boundaryPointLeft.x; j < boundaryPointRight.x; j++) {
      img.set(j, vertices[0].y + i, col);
    }

    // draw each segment line
    // artifacts
    // Line(boundaryPointLeft, boundaryPointRight, img, white);

    // draw the wire of the triangle
    Line(vertices[0], vertices[1], img, red);
    Line(vertices[0], vertices[2], img, red);
    Line(vertices[1], vertices[2], img, red);
  }
}

bool isAllGreater0(const int *arr, int size) {
  for (int i = 0; i < size; i++) {
    if (arr[i] < 0)
      return false;
  }

  return true;
}

bool isAllLess0(const int *arr, int size) {
  for (int i = 0; i < size; i++) {
    if (arr[i] > 0)
      return false;
  }

  return true;
}

bool isInBarycentric(const Vec2i &p, const Vec2f *vert) {
  int zValues[3] = {0};
  // vertices A,B and C are sorted from the lowest y value to the highest
  for (int i = 0; i < 3; i++) {
    // we don't have cast func from vec2i to vec2f
    Vec2f endpoint2p = Vec2f(p.x, p.y) - Vec2f(vert[i].x, vert[i].y);
    Vec2f edge = Vec2f(vert[(i + 1) % 3].x, vert[(i + 1) % 3].y) -
                 Vec2f(vert[i].x, vert[i].y);
    // convert them into 3d as cross product doesn't make scene in 2d
    Vec3f endpoint2p3d = Vec3f(endpoint2p.x, endpoint2p.y, 1.0);
    Vec3f edge3d = Vec3f(edge.x, edge.y, 1);
    // do the cross product
    Vec3f result = endpoint2p3d ^ edge3d;
    // store the z value of all corss product
    zValues[i] = result.z;
  }

  if (isAllGreater0(zValues, 3) || isAllLess0(zValues, 3))
    return true;

  return false;
}

float *barycentricCoords(const Vec2i &p, const Vec2f *vertices) {
  // we assume that the vertices is sorted counter clockwise
  if (isInBarycentric(p, vertices)) {
    // u,v,w are equal to the ration of the corresponding triangle
    float *weight;

    // trun 2d into 3d
    Vec3f p3d = Vec3f(p.x, p.y, 1.0);
    Vec3f *verts3d;

    for (int i = 0; i < 3; i++) {
      verts3d[i] = Vec3f(vertices[i].x, vertices[i].y, 1.0);
    }

    // use cross product to compute the barycentric coordinates
    // Cabc: cross product for triangle abc
    Vec3f Cabc = (verts3d[2] - verts3d[0]) ^ (verts3d[1] - verts3d[0]);
    // Sabc: area of triangle abc
    float Sabc = std::sqrt(Cabc.x * Cabc.x + Cabc.y * Cabc.y + Cabc.z * Cabc.z);
    for (int i = 0; i < 3; i++) {
      Vec3f endpoint2p = p3d - verts3d[i];
      Vec3f edge = verts3d[(i + 1) % 3] - verts3d[i];
      Vec3f CrossP = endpoint2p ^ edge;
      // Spart: part area
      float Spart = std::sqrt(CrossP.x * CrossP.x + CrossP.y * CrossP.y +
                              CrossP.z * CrossP.z);
      weight[i] = Spart / Sabc;
    }

    return weight;
  }

  std::cout << "the vertex is not in the triangle\n";
  return 0;
}

void drawBoundingBox(const Vec2i &lt, const Vec2i &rb, TGAImage &img,
                     const TGAColor &col) {
  Vec2i lb = Vec2i(lt.x, rb.y);
  Vec2i rt = Vec2i(rb.x, lt.y);
  Line(lt, lb, img, col);
  Line(lb, rb, img, col);
  Line(rb, rt, img, col);
  Line(rt, lt, img, col);
}

// problem must be here
void TriangleBarycentric(Vec2f *vertices, float *zBuffer, TGAImage &img,
                         const TGAColor &col) {
  // find the bounding box
  Vec2i leftTop =
      Vec2i(std::min(std::min(vertices[0].x, vertices[1].x), vertices[2].x),
            std::max(std::max(vertices[0].y, vertices[1].y), vertices[2].y));
  Vec2i rightBot =
      Vec2i(std::max(std::max(vertices[0].x, vertices[1].x), vertices[2].x),
            std::min(std::min(vertices[0].y, vertices[1].y), vertices[2].y));

  std::cout << "left top: " << leftTop.x << " " << leftTop.y << std::endl;

  // draw the bounding box
  drawBoundingBox(leftTop, rightBot, img, green);

  // loop the bounding box of a tri to determine if the pixel is inside the
  // tri filled the tri rows by rows
  for (int i = leftTop.y; i > rightBot.y; i--) {
    for (int j = leftTop.x; j < rightBot.x; j++) {
      if (isInBarycentric(Vec2i(j, i), vertices)) {
        img.set(j, i, col);
      }
    }
  }
}

void drawModel(Model *model, float *zBuffer, TGAImage &img, int drawMode) {
  switch (drawMode) {
  case drawMode::wireframe:
    // extra faces and vertices from models
    for (int i = 0; i < model->nfaces(); i++) {
      // get face
      std::vector<int> singleFace = model->face(i);
      // get vertices to draw the line
      for (int j = 0; j < 3; j++) {
        // loop to find the v0, v1 and v2
        Vec3f v0 = model->vert(singleFace[j]);
        Vec3f v1 = model->vert(singleFace[(j + 1) % 3]);
        // remap the data in v0 and v1 from -1~1 to 0~1
        int x0 = remap(v0.x, Vec2i(-1, 1), Vec2i(0, width));
        int x1 = remap(v1.x, Vec2i(-1, 1), Vec2i(0, width));
        int y0 = remap(v0.y, Vec2i(-1, 1), Vec2i(0, height));
        int y1 = remap(v1.y, Vec2i(-1, 1), Vec2i(0, height));
        Line(Vec2i(x0, y0), Vec2i(x1, y1), img, white);
      }
    }
    break;
  case drawMode::filledTri:
    for (int i = 0; i < model->nfaces(); i++) {
      // get face
      std::vector<int> singleFace = model->face(i);
      // get vertices to draw the line
      Vec2f screenCoords[3];
      Vec3f worldCoords[3];
      for (int j = 0; j < 3; j++) {
        // loop to find the v0, v1 and v2
        Vec3f v = model->vert(singleFace[j]);
        // remap the data in v0 and v1 from -1~1 to 0~width or 0~height
        int x = remap(v.x, Vec2i(-1, 1), Vec2i(0, width));
        int y = remap(v.y, Vec2i(-1, 1), Vec2i(0, height));

        screenCoords[j] = Vec2f(x, y);
        worldCoords[j] = v;
      }

      // calculate the normal
      Vec3f norm =
          (worldCoords[2] - worldCoords[0]) ^ (worldCoords[1] - worldCoords[0]);
      norm.normalize();

      // calculate the light effect
      float lightIntensity = norm * lightDir;
      TGAColor col = TGAColor(255 * lightIntensity, 255 * lightIntensity,
                              255 * lightIntensity, 255);

      // draw the triangle
      // since we don't do z test here, so we have to remove the face that the
      // light can't reach
      if (lightIntensity > 0) {
        // wrong: worldCoords is range from -1 to 1
        // TriangleBarycentric(worldCoords, zBuffer, img, col);
        TriangleBarycentric(screenCoords, zBuffer, img, col);
      }
    }
    break;
  default:
    std::cout << "please enter the proper drawMode" << std::endl;
    break;
  }
}

// -- z buffer
void updateZBuffer(float *zBuffer, int x, int y, float value) {
  zBuffer[x + y * width] = value;
}

void cleanZBuffer(float *zBuffer) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int idx = x + width * y;
      zBuffer[idx] = std::numeric_limits<float>::min();
    }
  }
}
