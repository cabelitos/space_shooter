#include <stdio.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"

int main(int argc, char **argv) {
  ALLEGRO_DISPLAY_MODE mode;
  GAME *game;
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
 
  if (!al_get_display_mode(al_get_num_display_modes() - 1, &mode)) {
    printf("Could not retrive the display mode!\n");
    return -1;
  }

  game = game_init(mode);
  if (!game) {
    printf("Could not init the game!\n");
    return -1;
  }
  game_run(game);
  game_shutdown(game);
  al_shutdown_primitives_addon();
  return 0;
}
