#pragma once

#include "utils.h"
#include "cannon.h"

typedef struct _SPACE_SHIP SPACE_SHIP;

SPACE_SHIP *space_ship_create(POINT display);

void space_ship_notify_keys(SPACE_SHIP *ss, KEYS pressed_keys);

CANNON *space_ship_get_cannon(SPACE_SHIP *ss);

unsigned space_ship_detect_collisions(SPACE_SHIP *ss,
				      OBJECT_POSITION *asteroids,
				      unsigned asteroids_size);

void space_ship_draw(SPACE_SHIP *ss);

void space_ship_destroy(SPACE_SHIP *ss);
