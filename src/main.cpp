#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <cmath>
#include <vector>

/* constants declaration */
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
// -- size of the image
const int width = 100;
const int height = 100;

/* variables declaration */

/* functions declaration */
// -- line func
void Line(const Vec2f &, const Vec2f &, TGAImage &, const TGAColor &);

int main(int argc, char **argv) {
  TGAImage image(width, height, TGAImage::RGB);
  image.flip_vertically(); // i want to have the origin at the left bottom
                           // corner of the image

  /* draw dots in the images */
  // image.set(52, 41, red);

  /* draw lines in the image */
  const Vec2f s = Vec2f(13, 20);
  const Vec2f e = Vec2f(80, 40);
  Line(s, e, image, white);

  image.write_tga_file("output.tga");
  return 0;
}

/* functions definition */
void Line(const Vec2f &start, const Vec2f &end, TGAImage &img,
          const TGAColor &col) {
  for (int x = start.x; x < end.x; x++) {
    float t = (x - start.x) / (float)(end.x - start.x);
    int y = start.y * (1.0 - t) + end.y * t;
    img.set(x, y, col);
  }
}
