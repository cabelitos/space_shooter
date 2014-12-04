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

#ifndef M_PI

#define M_PI 3.14159265358979323846264338327

#endif

POINT rotate_point(POINT p, POINT center, float degrees);
bool isInvisible(POINT p, POINT display);
