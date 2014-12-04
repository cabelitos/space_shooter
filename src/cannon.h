#pragma once

#include "utils.h"

typedef struct _CANNON CANNON;

CANNON *cannon_create(POINT display);

void cannon_shoot(CANNON *c, POINT origin, float rotation, KEYS pressed_keys);

void cannon_draw(CANNON *c);

void cannon_destroy(CANNON *cannon);
