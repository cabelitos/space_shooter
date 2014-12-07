#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "cannon.h"
#include "float_compare.h"

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
    c->lasers[i].pos->id = i;
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

unsigned *cannon_detect_collisions(CANNON *c,
				   OBJECT_POSITION *asteroids,
				   unsigned asteroids_size) {
  unsigned i, j, k, aNext, removedI;
  float minX, maxX, minY, maxY;
  unsigned *remove;

  if (!c)
    return NULL;

  remove = malloc(sizeof(unsigned) * asteroids_size);
  if (!remove)
    return NULL;
  removedI = 0;
  for (i = 0; i < asteroids_size; i++)
    remove[i] = UINT_MAX;

  for (i = 0; i < asteroids_size; i++) {
    for (j = 0; j < asteroids[i].points_size; j++) {
      aNext = j + 1;
      if (aNext == asteroids[i].points_size)
	aNext = 0;

      minX = fminf(asteroids[i].points[j].x, asteroids[i].points[aNext].x);
      maxX = fmaxf(asteroids[i].points[j].x, asteroids[i].points[aNext].x);
      minY = fminf(asteroids[i].points[j].y, asteroids[i].points[aNext].y);
      maxY = fmaxf(asteroids[i].points[j].y, asteroids[i].points[aNext].y);

      for (k = 0; k < MAX_LASERS_AT_ONCE; k++) {
	if (!c->lasers[k].valid)
	  continue;
	if ((isGreater(c->lasers[k].pos->points[0].x, minX)
	     && isLess(c->lasers[k].pos->points[0].x, maxX)
	     && isGreater(c->lasers[k].pos->points[0].y, minY)
	     && isLess(c->lasers[k].pos->points[0].y, maxY))
	    ||
	    (isGreater(c->lasers[k].pos->points[1].x, minX)
	     && isLess(c->lasers[k].pos->points[1].x, maxX)
	     && isGreater(c->lasers[k].pos->points[1].y, minY)
	     && isLess(c->lasers[k].pos->points[1].y, maxY))) {
	  c->lasers[k].valid = false;
	  c->totalLasers--;
	  remove[removedI++] = asteroids[i].id;
	}
      }
    }
  }
  return remove;
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
