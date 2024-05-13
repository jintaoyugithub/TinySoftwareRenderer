#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <cmath>
#include <iostream>
#include <vector>

/* constants declaration */
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
// -- size of the image
const int width = 800;
const int height = 800;

/* variables declaration */
Model *model = nullptr;

/* functions declaration */
// -- draw line func
void Line(Vec2i, Vec2i, TGAImage &, const TGAColor &);
// -- remap func
int remap(float, Vec2i, Vec2i);
// -- draw triangles func
void Triangle(std::vector<Vec2i> &, TGAImage &, const TGAColor &);

int main(int argc, char **argv) {
  TGAImage image(100, 100, TGAImage::RGB);

  /* draw triangle */
  std::vector<Vec2i> tri1 = {Vec2i(10, 20), Vec2i(50, 40), Vec2i(70, 80)};
  Triangle(tri1, image, white);
  image.flip_vertically();
  image.write_tga_file("output/triangle.tga");

  /* draw models */
  TGAImage humanHeadImage(width, height, TGAImage::RGB);
  // load models
  model = new Model("../models/obj/humanHead.obj");
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
      // Line(Vec2f(v0.x, v0.y), Vec2f(v1.x, v1.y), humanHeadImage, white);
      Line(Vec2i(x0, y0), Vec2i(x1, y1), humanHeadImage, white);
    }
  }

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

void Triangle(std::vector<Vec2i> &vertices, TGAImage &img,
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
    float t = isSecond ? 1 : (float)i / totalHeight;
    // determine the start point of the boundary
    Vec2i boundaryPointLeft = vertices[0] * (1 - t) + vertices[2] * t;
    Vec2i boundaryPointRight = isSecond
                                   ? vertices[0] * (1 - t) + vertices[1] * t
                                   : vertices[1] * (1 - t) + vertices[2] * t;

    // make sure it's render from left to right
    if (boundaryPointLeft.x > boundaryPointRight.x)
      std::swap(boundaryPointLeft, boundaryPointRight);
    for (int j = boundaryPointLeft.x; j < boundaryPointRight.x; j++) {
      img.set(j, vertices[0].y + i, col);
    }

    // draw each segment line
    // artifacts
    // Line(boundaryPointLeft, boundaryPointRight, img, white);
  }
}
