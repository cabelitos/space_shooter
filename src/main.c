#include <stdio.h>
#include <allegro5/allegro_primitives.h>

#include "game.h"

int main(int argc, char **argv) {
  ALLEGRO_MONITOR_INFO info;
  GAME *game;
  POINT p;
  (void) argc;
  (void) argv;

  if (!al_init()) {
    printf("Could not init allegro!\n");
    return -1;
  }

  if (!al_install_keyboard()) {
    printf("Could not setup the keyboard!\n");
    return -1;
  }

  if (!al_init_primitives_addon()) {
    printf("Could not init the primitives addon!\n");
    return -1;
  }

  if (!al_get_monitor_info(al_get_num_video_adapters() - 1, &info)) {
    printf("Could not retrive the display mode!\n");
    return -1;
  }

  p.x = info.x2 - info.x1;
  p.y = info.y2 - info.y1;

  game = game_init(p);
  if (!game) {
    printf("Could not init the game!\n");
    return -1;
  }
  game_run(game);
  game_shutdown(game);
  al_shutdown_primitives_addon();
  return 0;
}
