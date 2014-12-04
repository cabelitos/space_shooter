#pragma once

#include <allegro5/allegro.h>
#include "utils.h"

typedef struct _GAME GAME;

GAME *game_init(POINT display);
void game_run(GAME *game);
void game_shutdown(GAME *game);
