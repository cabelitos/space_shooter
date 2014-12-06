#pragma once

#include "utils.h"

typedef struct _ASTEROIDS_COORDINATOR ASTEROIDS_COORDINATOR;

ASTEROIDS_COORDINATOR *asteroids_coordinator_create(POINT display);
void asteroids_coordinator_move_asteroids(ASTEROIDS_COORDINATOR *ac);
void asteroids_coordinator_draw(ASTEROIDS_COORDINATOR *ac);
void asteroids_coordinator_destroy(ASTEROIDS_COORDINATOR *ac);
