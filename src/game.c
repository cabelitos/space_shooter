#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "utils.h"
#include "game.h"
#include "space_ship.h"
#include "cannon.h"
#include "asteroids_coordinator.h"

#define FPS (60)
#define FONT_SIZE (72)

struct _GAME {
  ALLEGRO_DISPLAY *display;
  ALLEGRO_EVENT_QUEUE *events;
  ALLEGRO_TIMER *timer;
  SPACE_SHIP *ss;
  ASTEROIDS_COORDINATOR *ac;
  ALLEGRO_FONT *font;
};

static ALLEGRO_FONT *_load_font(void) {
  ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
  if (!path) {
    printf("Path is null!\n");
    return NULL;
  }
  al_append_path_component(path, "res");
  al_append_path_component(path, "fonts");
  al_change_directory(al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP));
  al_destroy_path(path);
  return al_load_ttf_font("Times.ttf", FONT_SIZE, 0);
}

GAME *game_init(POINT display) {
  GAME *game = calloc(1, sizeof(GAME));

  if (!game)
    return NULL;

  game->display = al_create_display(display.x, display.y);
  if (!game->display)
    goto err_display;

  game->events = al_create_event_queue();

  if (!game->events)
    goto err_events;

  game->ss = space_ship_create(display);

  if (!game->ss)
    goto err_ss;

  game->timer = al_create_timer(1.0/FPS);

  if (!game->timer)
    goto err_timer;

  game->ac = asteroids_coordinator_create(display);

  if (!game->ac)
    goto err_ac;

  game->font = _load_font();

  if (!game->font)
    goto err_font;

  al_register_event_source(game->events,
			   al_get_display_event_source(game->display));
  al_register_event_source(game->events,
			   al_get_keyboard_event_source());
  al_register_event_source(game->events,
			   al_get_timer_event_source(game->timer));

  al_set_window_title(game->display, "Space shooter");
  return game;

 err_font:
  asteroids_coordinator_destroy(game->ac);
 err_ac:
  al_destroy_timer(game->timer);
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

static bool _detect_collisions(GAME *game) {
  unsigned asteroids_size = 0, id, i;
  unsigned *toRemove;
  bool ship_collision = false;
  OBJECT_POSITION *asteroids =
    asteroids_coordinator_get_asteroids_positions(game->ac, &asteroids_size);

  toRemove = cannon_detect_collisions(space_ship_get_cannon(game->ss),
				      asteroids,
				      asteroids_size);
  if (toRemove) {
    for (i = 0; i < asteroids_size; i++) {
      if (toRemove[i] == UINT_MAX)
	break;
      asteroids_coordinator_remove_asteroid(game->ac, toRemove[i]);
    }
    free(toRemove);
    free(asteroids);
    asteroids =
      asteroids_coordinator_get_asteroids_positions(game->ac, &asteroids_size);
  }
  id = space_ship_detect_collisions(game->ss, asteroids,
				    asteroids_size);
  if (id != UINT_MAX) {
    asteroids_coordinator_remove_asteroid(game->ac, id);
    ship_collision = true;
  }
  free(asteroids);
  return ship_collision;
}

void game_run(GAME *game) {
  ALLEGRO_EVENT event;
  bool redraw = false, ship_collision = false;
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
    pressed_keys &= ~SPACE;
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
      case ALLEGRO_KEY_SPACE:
	pressed_keys |= SPACE;
	break;
      }
    }

    asteroids_coordinator_move_asteroids(game->ac);

    if (!ship_collision) {
      space_ship_notify_keys(game->ss, pressed_keys);
      ship_collision = _detect_collisions(game);
    }

    if (redraw && al_event_queue_is_empty(game->events)) {
      redraw = false;
      al_clear_to_color(al_map_rgb(0,0,0));
      space_ship_draw(game->ss);
      asteroids_coordinator_draw(game->ac);
      if (ship_collision) {
	al_draw_text(game->font,
		     al_map_rgb(255, 255, 255),
		     al_get_display_width(game->display)/2,
		     al_get_display_height(game->display)/2,
		     ALLEGRO_ALIGN_CENTRE,
		     "Game Over!");
      }
      al_flip_display();
    }
  }
}

void game_shutdown(GAME *game) {
  if (!game) {
    printf("Game is null!\n");
    return;
  }

  al_destroy_font(game->font);
  asteroids_coordinator_destroy(game->ac);
  al_destroy_timer(game->timer);
  space_ship_destroy(game->ss);
  al_destroy_event_queue(game->events);
  al_destroy_display(game->display);
  free(game);
}
