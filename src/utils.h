#pragma once

#include <math.h>
#include <stdbool.h>

typedef enum _KEYS {
  NOT_SET = 0,
  UP = 1,
  DOWN = 2,
  LEFT = 4,
  RIGHT = 8,
  SPACE = 16
} KEYS;

typedef struct _POINT {
  float x;
  float y;
} POINT;

typedef struct _OBJECT_POSITION {
  POINT *points;
  unsigned points_size;
  POINT center;
  float rotation;
  unsigned id;
} OBJECT_POSITION;

#ifndef M_PI

#define M_PI (3.14159265358979323846264338327f)

#endif

#define M_DOUBLE_PI ((M_PI) * 2.0f)

POINT rotate_point(POINT p, POINT center, float degrees);
bool isInvisible(POINT p, POINT display);

OBJECT_POSITION *object_position_create(unsigned points_size);
void object_position_destroy(OBJECT_POSITION *obj);
void object_position_rotate_points(OBJECT_POSITION *obj, float rads);
