#ifndef _2D_H
#define _2D_H

struct Dim2D {
  int w, h;
};

struct Point2D {
  int x, y;
};

typedef struct Point2D Point2D;
typedef struct Dim2D Dim2D;

typedef Point2D PixelPoint2D;
typedef Dim2D PixelDim2D;

/**
 * Computes the x position that *thing would have to be in order to be
 * rendered in the horizontal center of *within.
 */
inline int
hor_center_within(const Dim2D *thing,
                  const Dim2D *within)
{
  return (within->w - thing->w)/2;
}

#endif
