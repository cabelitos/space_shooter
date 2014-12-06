#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "cannon.h"

#define THICKNESS (3.0f)
#define LASER_HEIGHT (10.0f)
#define LASER_SPEED (20.0f)
#define MAX_LASERS_AT_ONCE (5)
#define NOPE (10000)

typedef struct _LASER {
  OBJECT_POSITION *pos;
  bool valid;
} LASER;

struct _CANNON {
  POINT display;
  unsigned totalLasers;
  LASER lasers[MAX_LASERS_AT_ONCE];
};

CANNON *cannon_create(POINT display) {
  unsigned i;
  CANNON *c = calloc(1, sizeof(CANNON));
  if (!c)
    return NULL;
  c->display = display;
  for (i = 0; i < MAX_LASERS_AT_ONCE; i++) {
    c->lasers[i].pos = object_position_create(2);
    if (!c->lasers[i].pos)
      goto err_pos;
  }
  return c;

 err_pos:
  for (i = 0; i < MAX_LASERS_AT_ONCE; i++)
    object_position_destroy(c->lasers[i].pos);
  free(c);
  return NULL;
}

static void _move_lasers(CANNON *c) {
  unsigned i, ignoring = 0;
  for (i = 0; i < MAX_LASERS_AT_ONCE; i++) {
    if (!c->lasers[i].valid)
      continue;
    c->lasers[i].pos->points[0].x += sinf(c->lasers[i].pos->rotation) * LASER_SPEED;
    c->lasers[i].pos->points[0].y -= cosf(c->lasers[i].pos->rotation) * LASER_SPEED;
    c->lasers[i].pos->center = c->lasers[i].pos->points[0];
    c->lasers[i].pos->points[1].x = c->lasers[i].pos->points[0].x;
    c->lasers[i].pos->points[1].y = c->lasers[i].pos->points[0].y + LASER_HEIGHT;

    object_position_rotate_points(c->lasers[i].pos, c->lasers[i].pos->rotation);

    if (isInvisible(c->lasers[i].pos->points[0], c->display) &&
	isInvisible(c->lasers[i].pos->points[1], c->display)) {
      c->lasers[i].valid = false;
      ignoring++;
    }
  }
  c->totalLasers -= ignoring;
}

static unsigned _find_first_available_index(CANNON *c) {
  unsigned i;
  for (i = 0; i < MAX_LASERS_AT_ONCE; i++)
    if (!c->lasers[i].valid)
      return i;
  return NOPE;
}

void cannon_shoot(CANNON *c, POINT origin, float rotation,
		  KEYS pressed_keys) {
  unsigned i;
  if (!c) {
    printf("Cannon is null\n");
    return;
  }
  if ((pressed_keys & SPACE) &&
      (c->totalLasers < MAX_LASERS_AT_ONCE)) {
    i = _find_first_available_index(c);
    if (i != NOPE) {
      c->lasers[i].pos->center = c->lasers[i].pos->points[0]
      = c->lasers[i].pos->points[1] = origin;
      c->lasers[i].pos->rotation = rotation;
      c->lasers[i].valid = true;
      c->totalLasers++;
    } else
      printf("Something is wrong, could not find a index!\n");
  }
  _move_lasers(c);
}

void cannon_draw(CANNON *c) {
  unsigned i;
  if (!c) {
    printf("Cannon is null\n");
    return;
  }

  for (i = 0; i < MAX_LASERS_AT_ONCE; i++) {
    if (!c->lasers[i].valid)
      continue;
    al_draw_line(c->lasers[i].pos->points[0].x,
		 c->lasers[i].pos->points[0].y,
		 c->lasers[i].pos->points[1].x,
		 c->lasers[i].pos->points[1].y,
		 al_map_rgb(255, 0, 0),
		 THICKNESS);
  }
}

void cannon_destroy(CANNON *cannon) {
  unsigned i;
  for (i = 0; i < MAX_LASERS_AT_ONCE; i++)
    object_position_destroy(cannon->lasers[i].pos);
  free(cannon);
}
