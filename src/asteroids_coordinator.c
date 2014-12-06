#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "asteroids_coordinator.h"
#include "float_compare.h"

#define MAX_ASTEROIDS (6)
#define MAX_ASTEROID_SPEED (3)
#define ASTEROID_POINTS (6)

typedef struct _ASTEROID {
  POINT points[ASTEROID_POINTS];
  POINT center;
  float x_speed;
  float y_speed;
  float rotation_step;
  bool valid;
  float rotation;
} ASTEROID;

struct _ASTEROIDS_COORDINATOR {
  POINT display;
  ASTEROID asteroids[MAX_ASTEROIDS];
};

ASTEROIDS_COORDINATOR *asteroids_coordinator_create(POINT display) {
  ASTEROIDS_COORDINATOR *ac = calloc(1, sizeof(ASTEROIDS_COORDINATOR));

  if (!ac)
    return NULL;

  srand(time(NULL));
  ac->display = display;
  return ac;
}

static bool _set_asteroid_position(ASTEROID *asteroid,
				   POINT center,
				   POINT display,
				   float rotation) {
  unsigned p, invisiblePoints;

  p = invisiblePoints = 0;
  //FIXME: Create a better looking asteroid.
  asteroid->points[p] = asteroid->center;
  asteroid->points[p].y -= 20;

  p++;
  asteroid->points[p].x = asteroid->points[p-1].x - 30;
  asteroid->points[p].y = asteroid->points[p-1].y + 10;

  p++;
  asteroid->points[p].x = asteroid->points[p-1].x;
  asteroid->points[p].y = asteroid->points[p-1].y + 20;

  p++;
  asteroid->points[p].x = asteroid->points[p-1].x + 30;
  asteroid->points[p].y = asteroid->points[p-1].y + 20;

  p++;
  asteroid->points[p].x = asteroid->points[p-1].x + 30;
  asteroid->points[p].y = asteroid->points[p-1].y - 20;

  p++;
  asteroid->points[p].x = asteroid->points[p-1].x;
  asteroid->points[p].y = asteroid->points[p-1].y - 20;

  //now rotate
  for (p = 0; p < ASTEROID_POINTS; p++) {
    asteroid->points[p] =  rotate_point(asteroid->points[p], center, rotation);
    if (isInvisible(asteroid->points[p], display))
      invisiblePoints++;
  }
  return invisiblePoints != ASTEROID_POINTS;
}

void asteroids_coordinator_move_asteroids(ASTEROIDS_COORDINATOR *ac) {
  unsigned i;

  if (!ac)
    return;

  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (!ac->asteroids[i].valid) {
      ac->asteroids[i].valid = true;
      ac->asteroids[i].x_speed = (rand() % MAX_ASTEROID_SPEED) +
	(double)MAX_ASTEROID_SPEED/4;
      ac->asteroids[i].y_speed = (rand() % MAX_ASTEROID_SPEED) +
	(double)MAX_ASTEROID_SPEED/4;
      if (rand() % 2) {
	if (rand() % 2)
	  ac->asteroids[i].center.x = 0.0f;
	else
	  ac->asteroids[i].center.x = ac->display.x;

	ac->asteroids[i].center.y = rand() % (int)ac->display.y;
      } else {
	ac->asteroids[i].center.x = rand() % (int)ac->display.x;
	if (rand() % 2)
	  ac->asteroids[i].center.y = 0.0f;
	else
	  ac->asteroids[i].center.y = ac->display.y;
      }
      ac->asteroids[i].rotation = 0.0f;
      ac->asteroids[i].rotation_step = (rand() % 2) * (M_PI / 50.0f) + 0.05;

      if ((int)ac->asteroids[i].x_speed % 2)
	ac->asteroids[i].rotation_step *= -1;
    }

    ac->asteroids[i].rotation += ac->asteroids[i].rotation_step;
    if (isGreater(fabs(ac->asteroids[i].rotation), M_DOUBLE_PI))
      ac->asteroids[i].rotation = 0.0;

    ac->asteroids[i].center.x += ac->asteroids[i].x_speed;

    ac->asteroids[i].center.y += ac->asteroids[i].y_speed;

    ac->asteroids[i].valid = _set_asteroid_position(&ac->asteroids[i],
						    ac->asteroids[i].center,
						    ac->display,
						    ac->asteroids[i].rotation);
  }
}

void asteroids_coordinator_draw(ASTEROIDS_COORDINATOR *ac) {
  unsigned i;

  if (!ac)
    return;

  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (!ac->asteroids[i].valid)
      continue;
    al_draw_filled_polygon((float*)ac->asteroids[i].points,
			   ASTEROID_POINTS,
			   al_map_rgb(120, 120, 120));
  }
}

void asteroids_coordinator_destroy(ASTEROIDS_COORDINATOR *ac) {
  free(ac);
}
