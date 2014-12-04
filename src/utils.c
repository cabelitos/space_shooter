#include <math.h>

#include "utils.h"
#include "float_compare.h"

bool isInvisible(POINT p, POINT display) {
  return isGreater(p.x, display.x) || isGreater(p.y, display.y) ||
    isLess(p.x, 0.0) || isLess(p.y, 0.0);
}

POINT rotate_point(POINT p, POINT center, float degrees) {
  POINT aux;

  p.x -= center.x;
  p.y -= center.y;

  aux.x = (p.x * cosf(degrees)) - (p.y * sinf(degrees));
  aux.y = (p.x * sinf(degrees)) + (p.y * cosf(degrees));

  aux.x += center.x;
  aux.y += center.y;

  return aux;
}
