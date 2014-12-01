#pragma once
#include <allegro5/allegro.h>

typedef struct _GAME GAME;

GAME *game_init(ALLEGRO_DISPLAY_MODE mode);
void game_run(GAME *game);
void game_shutdown(GAME *game);
