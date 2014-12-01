#pragma once

#include "keys.h"

typedef struct _SPACE_SHIP SPACE_SHIP;

SPACE_SHIP *space_ship_create(int display_width, int display_height);

void space_ship_move(SPACE_SHIP *ss, KEYS pressed_keys);

void space_ship_rotate(SPACE_SHIP *ss, KEYS pressed_keys);

void space_ship_draw(SPACE_SHIP *ss);

void space_ship_destroy(SPACE_SHIP *ss);
