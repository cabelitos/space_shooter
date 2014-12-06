#include <math.h>
#include <stdlib.h>

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

OBJECT_POSITION *object_position_create(unsigned points_size) {
  OBJECT_POSITION *obj = calloc(1, sizeof(OBJECT_POSITION));
  if (!obj)
    return NULL;
  obj->points_size = points_size;
  obj->points = calloc(obj->points_size, sizeof(POINT));
  return obj;
}

void object_position_destroy(OBJECT_POSITION *obj) {
  if (!obj)
    return;
  free(obj->points);
  free(obj);
}

void object_position_rotate_points(OBJECT_POSITION *obj, float rads) {
  unsigned i;
  for (i = 0; i < obj->points_size; i++)
    obj->points[i] = rotate_point(obj->points[i], obj->center, rads);
}
