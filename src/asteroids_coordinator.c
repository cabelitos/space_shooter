#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "asteroids_coordinator.h"
#include "float_compare.h"

#define MAX_ASTEROIDS (6)
#define MAX_ASTEROID_SPEED (3)
#define ASTEROID_POINTS (6)

typedef struct _ASTEROID {
  OBJECT_POSITION *pos;
  float x_speed;
  float y_speed;
  float rotation_step;
  bool valid;
} ASTEROID;

struct _ASTEROIDS_COORDINATOR {
  POINT display;
  ASTEROID asteroids[MAX_ASTEROIDS];
};

ASTEROIDS_COORDINATOR *asteroids_coordinator_create(POINT display) {
  unsigned i;
  ASTEROIDS_COORDINATOR *ac = calloc(1, sizeof(ASTEROIDS_COORDINATOR));

  if (!ac)
    return NULL;

  for (i = 0; i < MAX_ASTEROIDS; i++) {
    ac->asteroids[i].pos = object_position_create(ASTEROID_POINTS);
    if (!ac->asteroids[i].pos)
      goto err_pos;
    ac->asteroids[i].pos->id = i;
  }

  srand(time(NULL));
  ac->display = display;
  return ac;

 err_pos:
  for (i = 0; i < MAX_ASTEROIDS; i++)
    object_position_destroy(ac->asteroids[i].pos);
  free(ac);
  return NULL;
}

static bool _set_asteroid_position(ASTEROID *asteroid,
				   POINT display) {
  unsigned p, invisiblePoints;

  p = invisiblePoints = 0;
  //FIXME: Create a better looking asteroid.
  asteroid->pos->points[p] = asteroid->pos->center;
  asteroid->pos->points[p].y -= 20;

  p++;
  asteroid->pos->points[p].x = asteroid->pos->points[p-1].x - 30;
  asteroid->pos->points[p].y = asteroid->pos->points[p-1].y + 10;

  p++;
  asteroid->pos->points[p].x = asteroid->pos->points[p-1].x;
  asteroid->pos->points[p].y = asteroid->pos->points[p-1].y + 20;

  p++;
  asteroid->pos->points[p].x = asteroid->pos->points[p-1].x + 30;
  asteroid->pos->points[p].y = asteroid->pos->points[p-1].y + 20;

  p++;
  asteroid->pos->points[p].x = asteroid->pos->points[p-1].x + 30;
  asteroid->pos->points[p].y = asteroid->pos->points[p-1].y - 20;

  p++;
  asteroid->pos->points[p].x = asteroid->pos->points[p-1].x;
  asteroid->pos->points[p].y = asteroid->pos->points[p-1].y - 20;

  object_position_rotate_points(asteroid->pos, asteroid->pos->rotation);

  for (p = 0; p < ASTEROID_POINTS; p++) {
    if (isInvisible(asteroid->pos->points[p], display))
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
	  ac->asteroids[i].pos->center.x = 0.0f;
	else
	  ac->asteroids[i].pos->center.x = ac->display.x;

	ac->asteroids[i].pos->center.y = rand() % (int)ac->display.y;
      } else {
	ac->asteroids[i].pos->center.x = rand() % (int)ac->display.x;
	if (rand() % 2)
	  ac->asteroids[i].pos->center.y = 0.0f;
	else
	  ac->asteroids[i].pos->center.y = ac->display.y;
      }
      ac->asteroids[i].pos->rotation = 0.0f;
      ac->asteroids[i].rotation_step = (rand() % 2) * (M_PI / 50.0f) + 0.05;

      if ((int)ac->asteroids[i].x_speed % 2)
	ac->asteroids[i].rotation_step *= -1;
    }

    ac->asteroids[i].pos->rotation += ac->asteroids[i].rotation_step;
    if (isGreater(fabs(ac->asteroids[i].pos->rotation), M_DOUBLE_PI))
      ac->asteroids[i].pos->rotation = 0.0;

    ac->asteroids[i].pos->center.x += ac->asteroids[i].x_speed;

    ac->asteroids[i].pos->center.y += ac->asteroids[i].y_speed;

    ac->asteroids[i].valid = _set_asteroid_position(&ac->asteroids[i],
						    ac->display);
  }
}

OBJECT_POSITION *asteroids_coordinator_get_asteroids_positions(ASTEROIDS_COORDINATOR *ac,
							       unsigned *size) {
  unsigned i;
  OBJECT_POSITION *list = NULL, *aux;
  size_t total_len = 0, obj_size = sizeof(OBJECT_POSITION);

  if (!ac || !size)
    return list;

  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (!ac->asteroids[i].valid)
      continue;
    aux = realloc(list, total_len + obj_size);
    if (!aux)
      goto exit;
    list = aux;
    memcpy((void*)list + total_len, ac->asteroids[i].pos, obj_size);
    total_len += obj_size;
  }
  *size = total_len / obj_size;
  return list;
 exit:
  free(list);
  return NULL;
}

void asteroids_coordinator_remove_asteroid(ASTEROIDS_COORDINATOR *ac, unsigned id) {
  unsigned i;

  if (!ac)
    return;

  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (ac->asteroids[i].pos->id == id) {
      ac->asteroids[i].valid = false;
      break;
    }
  }
}

void asteroids_coordinator_draw(ASTEROIDS_COORDINATOR *ac) {
  unsigned i;

  if (!ac)
    return;

  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (!ac->asteroids[i].valid)
      continue;
    al_draw_filled_polygon((float*)ac->asteroids[i].pos->points,
			   ASTEROID_POINTS,
			   al_map_rgb(120, 120, 120));
  }
}

void asteroids_coordinator_destroy(ASTEROIDS_COORDINATOR *ac) {
  unsigned i;
  if (!ac)
    return;
  for (i = 0; i < MAX_ASTEROIDS; i++)
    object_position_destroy(ac->asteroids[i].pos);
  free(ac);
}
