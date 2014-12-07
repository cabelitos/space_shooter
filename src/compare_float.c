#include <float.h>
#include <math.h>
#include "compare_float.h"

#define EPSILON (FLT_EPSILON)

bool isEqual(float a, float b) {
  return (fabs(a - b) <= EPSILON * fmax(fabs(a), fabs(b)));
}

bool isGreater(float a, float b) {
  return ((a - b) > EPSILON * fmax(fabs(a), fabs(b)));
}

bool isLess(float a, float b) {
  return ((b - a) > EPSILON * fmax(fabs(a), fabs(b)));
}
