#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <cmath>
#include <iostream>
#include <vector>

/* constants declaration */
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
// -- draw mode
enum drawMode { wireframe, filledTri, filledTriLitted };
// -- size of the image
const int width = 800;
const int height = 800;
// -- light dir
const Vec3f lightDir(0, 0, -1);

/* global variables declaration */
Model *model = nullptr;
float *zBuffer;

/* functions declaration */
// -- draw line func
void Line(Vec2i, Vec2i, TGAImage &, const TGAColor &);
// -- draw triangles func, line sweep
void TriangleLineSweep(std::vector<Vec2i> &, TGAImage &, const TGAColor &);
// draw triangle with the barycentric method
void TriangleBarycentric(std::vector<Vec2i> &, TGAImage &, const TGAColor &);
void TriangleBarycentricWithZBuffer(Vec3f *, float *, TGAImage &,
                                    const TGAColor &);
// -- draw obj models
void drawModel(Model *, TGAImage &, int);

// - utilities function
// can be written to template funciton
void sortY(std::vector<Vec2i> &);
void sortY(std::vector<Vec3f> &);

bool isAllGreater0(const int *, int);
bool isAllLess0(const int *, int);
bool isInBarycentric(const Vec2i &, const std::vector<Vec2i> &);
// for 3d triangle
bool isInBarycentric(const Vec2i &, const Vec3f *);
// -- draw triangles func, barycentric coordinates
void drawBoundingBox(const Vec2i &, const Vec2i &, TGAImage &,
                     const TGAColor &);
// -- remap func
int remap(float, Vec2i, Vec2i);
// -- calculate barycentric coordinates of a point
Vec3f barycentricCoord(const Vec3f &, const std::vector<Vec3f> &);

int main(int argc, char **argv) {
  TGAImage image(100, 100, TGAImage::RGB);
  // load models
  model = new Model("../models/obj/humanHead.obj");

  /* draw filled human head with light */
  TGAImage humanHeadFilledWithLightImage(width, height, TGAImage::RGB);
  drawModel(model, humanHeadFilledWithLightImage, drawMode::filledTri);
  humanHeadFilledWithLightImage.flip_vertically();
  humanHeadFilledWithLightImage.write_tga_file(
      "output/filledHumanHeadWithLight.tga");

  /* draw filled human head */
  TGAImage humanHeadFilledImage(width, height, TGAImage::RGB);
  drawModel(model, humanHeadFilledImage, drawMode::filledTri);
  humanHeadFilledImage.flip_vertically();
  humanHeadFilledImage.write_tga_file("output/filledHumanHead.tga");

  /* draw triangle */
  TGAImage triangleImage(200, 200, TGAImage::RGB);
  std::vector<Vec2i> tri1 = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
  std::vector<Vec2i> tri2 = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
  std::vector<Vec2i> tri3 = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

  TriangleLineSweep(tri1, triangleImage, white);
  TriangleBarycentric(tri2, triangleImage, green);
  TriangleLineSweep(tri3, triangleImage, blue);
  triangleImage.flip_vertically();
  triangleImage.write_tga_file("output/barycentricTri.tga");

  /* draw models */
  TGAImage humanHeadImage(width, height, TGAImage::RGB);
  drawModel(model, humanHeadImage, drawMode::wireframe);

  humanHeadImage.flip_vertically(); // i want to have the origin at the left
                                    // bottom corner of the image
  humanHeadImage.write_tga_file("output/humanHead.tga");

  /* draw lines in the image */
  Line(Vec2i(13, 20), Vec2i(80, 40), image, white);
  Line(Vec2i(20, 13), Vec2i(40, 80), image, white);
  Line(Vec2i(80, 40), Vec2i(13, 20), image, red);

  image.flip_vertically(); // i want to have the origin at the left bottom
                           // corner of the image
  // image.write_tga_file("output/stable&steepLine.tga");
  image.write_tga_file("output/perfectStable&steepLine.tga");

  /* draw dots in the images */
  image.set(52, 41, red);
  image.write_tga_file("output/dots.tga");

  delete model;
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

bool isInBarycentric(const Vec2i &p, const std::vector<Vec2i> &vert) {
  if (vert.size() != 3) {
    std::cout << "wrong triangle sieze" << std::endl;
    return false;
  }
  int zValues[3] = {0};
  // vertices A,B and C are sorted from the lowest y value to the highest
  for (int i = 0; i < vert.size(); i++) {
    Vec2i endpoint2p = p - vert[i];
    Vec2i edge = vert[(i + 1) % 3] - vert[i];
    Vec3i endpoint2p3d = Vec3i(endpoint2p.x, endpoint2p.y, 1);
    Vec3i edge3d = Vec3i(edge.x, edge.y, 1);
    // do cross product
    Vec3i result = endpoint2p3d ^ edge3d;
    // store the z value of all corss product
    zValues[i] = result.z;
  }

  if (isAllGreater0(zValues, 3) || isAllLess0(zValues, 3))
    return true;

  return false;
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

void TriangleBarycentric(std::vector<Vec2i> &vert, TGAImage &img,
                         const TGAColor &col) {
  // find the bounding box
  Vec2i leftTop = Vec2i(std::min(std::min(vert[0].x, vert[1].x), vert[2].x),
                        std::max(std::max(vert[0].y, vert[1].y), vert[2].y));
  Vec2i rightBot = Vec2i(std::max(std::max(vert[0].x, vert[1].x), vert[2].x),
                         std::min(std::min(vert[0].y, vert[1].y), vert[2].y));

  // draw the bounding box
  drawBoundingBox(leftTop, rightBot, img, green);

  // loop the bounding box of a tri to determine if the pixel is inside the
  // tri filled the tri rows by rows
  for (int i = leftTop.y; i > rightBot.y; i--) {
    for (int j = leftTop.x; j < rightBot.x; j++) {
      if (isInBarycentric(Vec2i(j, i), vert)) {
        img.set(j, i, col);
      }
    }
  }
}

void drawModel(Model *model, TGAImage &img, int drawMode) {
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
      std::vector<Vec2i> triVertices;
      for (int j = 0; j < 3; j++) {
        // loop to find the v0, v1 and v2
        Vec3f v = model->vert(singleFace[j]);
        // remap the data in v0 and v1 from -1~1 to 0~width or 0~height
        int x = remap(v.x, Vec2i(-1, 1), Vec2i(0, width));
        int y = remap(v.y, Vec2i(-1, 1), Vec2i(0, height));
        triVertices.push_back(Vec2i(x, y));
      }
      // draw the triangle
      TriangleBarycentric(triVertices, img, white);
    }
    break;
  default:
    std::cout << "please enter the proper drawMode" << std::endl;
    break;
  }
}

void TriangleBarycentricWithZBuffer(Vec3f *vert, float *zBuffer, TGAImage &img,
                                    const TGAColor &col) {

  // find the bounding box
  Vec2i leftTop = Vec2i(std::min(std::min(vert[0].x, vert[1].x), vert[2].x),
                        std::max(std::max(vert[0].y, vert[1].y), vert[2].y));
  Vec2i rightBot = Vec2i(std::max(std::max(vert[0].x, vert[1].x), vert[2].x),
                         std::min(std::min(vert[0].y, vert[1].y), vert[2].y));

  // draw the bounding box
  drawBoundingBox(leftTop, rightBot, img, green);

  // loop the bounding box of a tri to determine if the pixel is inside the
  // tri filled the tri rows by rows
  for (int i = leftTop.y; i > rightBot.y; i--) {
    for (int j = leftTop.x; j < rightBot.x; j++) {
      // if not in the triangle
      if (!isInBarycentric(Vec2i(j, i), vert))
        continue;
    }
  }
}
