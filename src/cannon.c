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
  POINT p1;
  POINT p2;
  float rotation;
  bool valid;
} LASER;

struct _CANNON {
  POINT display;
  unsigned totalLasers;
  LASER lasers[MAX_LASERS_AT_ONCE];
};

CANNON *cannon_create(POINT display) {
  CANNON *c = calloc(1, sizeof(CANNON));
  if (!c)
    return NULL;
  c->display = display;
  return c;
}

static void _move_lasers(CANNON *c) {
  unsigned i, ignoring = 0;
  for (i = 0; i < MAX_LASERS_AT_ONCE; i++) {
    if (!c->lasers[i].valid)
      continue;
    c->lasers[i].p1.x += sinf(c->lasers[i].rotation) * LASER_SPEED;
    c->lasers[i].p1.y -= cosf(c->lasers[i].rotation) * LASER_SPEED;
    c->lasers[i].p2.x = c->lasers[i].p1.x;
    c->lasers[i].p2.y = c->lasers[i].p1.y + LASER_HEIGHT;
    c->lasers[i].p2 = rotate_point(c->lasers[i].p2,
				   c->lasers[i].p1,
				   c->lasers[i].rotation);
    if (isInvisible(c->lasers[i].p1, c->display) &&
	isInvisible(c->lasers[i].p2, c->display)) {
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
      c->lasers[i].p1 = origin;
      c->lasers[i].p2 = origin;
      c->lasers[i].rotation = rotation;
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
    al_draw_line(c->lasers[i].p1.x,
		 c->lasers[i].p1.y,
		 c->lasers[i].p2.x,
		 c->lasers[i].p2.y,
		 al_map_rgb(255, 0, 0),
		 THICKNESS);
  }
}

void cannon_destroy(CANNON *cannon) {
  free(cannon);
}
