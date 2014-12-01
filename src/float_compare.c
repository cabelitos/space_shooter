#include <float.h>
#include <math.h>
#include "float_compare.h"

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
