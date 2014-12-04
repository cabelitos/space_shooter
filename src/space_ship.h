#pragma once

#include "utils.h"

typedef struct _SPACE_SHIP SPACE_SHIP;

SPACE_SHIP *space_ship_create(POINT display);

void space_ship_notify_keys(SPACE_SHIP *ss, KEYS pressed_keys);

void space_ship_draw(SPACE_SHIP *ss);

void space_ship_destroy(SPACE_SHIP *ss);
