#include <stdlib.h>
#include <stdbool.h>

#include "keys.h"
#include "game.h"
#include "space_ship.h"

#define FPS (60)

struct _GAME {
  ALLEGRO_DISPLAY *display;
  ALLEGRO_EVENT_QUEUE *events;
  ALLEGRO_TIMER *timer;
  SPACE_SHIP *ss;
};

GAME *game_init(ALLEGRO_DISPLAY_MODE mode) {
  GAME *game = calloc(1, sizeof(GAME));

  if (!game)
    return NULL;

  game->display = al_create_display(mode.width, mode.height);
  if (!game->display)
    goto err_display;

  game->events = al_create_event_queue();

  if (!game->events)
    goto err_events;

  game->ss = space_ship_create(al_get_display_width(game->display),
			       al_get_display_height(game->display));

  if (!game->ss)
    goto err_ss;

  game->timer = al_create_timer(1.0/FPS);

  if (!game->timer)
    goto err_timer;

  al_register_event_source(game->events,
			   al_get_display_event_source(game->display));
  al_register_event_source(game->events,
			   al_get_keyboard_event_source());
  al_register_event_source(game->events,
			   al_get_timer_event_source(game->timer));

  al_set_window_title(game->display, "Space shooter");
  return game;

 err_timer:
  space_ship_destroy(game->ss);
 err_ss:
  al_destroy_event_queue(game->events);
 err_events:
  al_destroy_display(game->display);
 err_display:
  free(game);
  return NULL;
}

void game_run(GAME *game) {
  ALLEGRO_EVENT event;
  bool redraw = false;
  KEYS pressed_keys;
  if (!game) {
    printf("Game is null!\n");
    return;
  }

  al_clear_to_color(al_map_rgb(0,0,0));
  space_ship_draw(game->ss);
  al_flip_display();
  al_start_timer(game->timer);

  pressed_keys = NOT_SET;

  while (1) {
    al_wait_for_event(game->events, &event);
    if (event.type == ALLEGRO_EVENT_TIMER)
      redraw = true;
    else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
      break;
    else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
      switch (event.keyboard.keycode) {
      case ALLEGRO_KEY_UP:
	pressed_keys |= UP;
	break;
      case ALLEGRO_KEY_DOWN:
	pressed_keys |= DOWN;
	break;
      case ALLEGRO_KEY_LEFT:
	pressed_keys |= LEFT;
	break;
      case ALLEGRO_KEY_RIGHT:
	pressed_keys |= RIGHT;
	break;
      }
    } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
      switch (event.keyboard.keycode) {
      case ALLEGRO_KEY_UP:
	pressed_keys &= ~UP;
	break;
      case ALLEGRO_KEY_DOWN:
	pressed_keys &= ~DOWN;
	break;
      case ALLEGRO_KEY_LEFT:
	pressed_keys &= ~LEFT;
	break;
      case ALLEGRO_KEY_RIGHT:
	pressed_keys &= ~RIGHT;
	break;
      }
    }

    space_ship_notify_keys(game->ss, pressed_keys);

    if (redraw && al_event_queue_is_empty(game->events)) {
      redraw = false;
      al_clear_to_color(al_map_rgb(0,0,0));
      space_ship_draw(game->ss);
      al_flip_display();
    }
  }
}

void game_shutdown(GAME *game) {
  if (!game) {
    printf("Game is null!\n");
    return;
  }

  al_destroy_timer(game->timer);
  space_ship_destroy(game->ss);
  al_destroy_event_queue(game->events);
  al_destroy_display(game->display);
  free(game);
}
