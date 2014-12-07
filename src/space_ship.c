#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "compare_float.h"
#include "space_ship.h"

//This height should be scaled.
#define SPACE_SHIP_HEIGHT (24.0f)
#define SPACE_SHIP_BASE_WIDTH (10.0f)
#define SPACE_SHIP_HALF_HEIGHT ((SPACE_SHIP_HEIGHT)/2.0f)

#define SPACE_SHIP_ROTATION_STEP (0.104719755f)
#define MAX_ACCELERATION (10.0f)
#define ACCELIERATION_STEP (0.02f)

struct _SPACE_SHIP {
  OBJECT_POSITION *pos;

  POINT display;
  bool draw;

  float acceleration;

  CANNON *c;
};

static void _set_position(SPACE_SHIP *ss, float x, float y) {

  if (isGreater(y, ss->display.y))
    ss->pos->center.y = 0.0f;
  else if (isLess(y, 0.0))
    ss->pos->center.y = ss->display.y;
  else
    ss->pos->center.y = y;

  if (isGreater(x, ss->display.x))
    ss->pos->center.x = 0.0f;
  else if (isLess(x, 0.0))
    ss->pos->center.x = ss->display.x;
  else
    ss->pos->center.x = x;

  ss->pos->points[0].x = ss->pos->center.x;
  ss->pos->points[0].y = ss->pos->center.y - SPACE_SHIP_HALF_HEIGHT;

  ss->pos->points[1].x = ss->pos->center.x - SPACE_SHIP_BASE_WIDTH;
  ss->pos->points[1].y = ss->pos->center.y + SPACE_SHIP_HALF_HEIGHT;

  ss->pos->points[2].x = ss->pos->center.x + SPACE_SHIP_BASE_WIDTH;
  ss->pos->points[2].y = ss->pos->center.y + SPACE_SHIP_HALF_HEIGHT;

  object_position_rotate_points(ss->pos, ss->pos->rotation);
}

SPACE_SHIP *space_ship_create(POINT display) {
  SPACE_SHIP *ss = calloc(1, sizeof(SPACE_SHIP));
  if (!ss)
    return NULL;

  ss->display = display;
  ss->pos = object_position_create(3);

  if (!ss->pos)
    goto err_pos;
  ss->pos->id = 0;
  _set_position(ss, display.x/2.0f, display.y/2.0f);

  ss->c = cannon_create(ss->display);
  if (!ss->c)
    goto err_cannon;

  ss->draw = true;
  return ss;

 err_cannon:
  object_position_destroy(ss->pos);
 err_pos:
  free(ss);
  return NULL;
}

static void _space_ship_move(SPACE_SHIP *ss, KEYS pressed_keys) {
  POINT p;
  bool automaticStopDown = false;
  bool automaticStopUp = false;

  p = ss->pos->center;

  if (pressed_keys & UP) {
    ss->acceleration += ACCELIERATION_STEP;
  } else if (pressed_keys & DOWN) {
    ss->acceleration -= ACCELIERATION_STEP;
  } else if (isGreater(ss->acceleration, 0.0f)) {
    ss->acceleration -= ACCELIERATION_STEP;
    automaticStopUp = true;
  } else if (isLess(ss->acceleration, 0.0f)) {
    ss->acceleration += ACCELIERATION_STEP;
    automaticStopDown = true;
  }

  if ((automaticStopUp && isLess(ss->acceleration, 0.0f)) ||
      (automaticStopDown && isGreater(ss->acceleration, 0.0f)))
    ss->acceleration = 0.0f;
  else if (isGreater(fabs(ss->acceleration), MAX_ACCELERATION))
    ss->acceleration = MAX_ACCELERATION;

  p.x += sinf(ss->pos->rotation) * ss->acceleration;
  p.y -= cosf(ss->pos->rotation) * ss->acceleration;

  if (!isEqual(p.x, ss->pos->center.x) || !isEqual(p.y, ss->pos->center.y))
    _set_position(ss, p.x, p.y);
}

static void _space_ship_rotate(SPACE_SHIP *ss, KEYS pressed_keys) {
  float rads;

  if (pressed_keys & LEFT)
    rads = -SPACE_SHIP_ROTATION_STEP;
  else if (pressed_keys & RIGHT)
    rads = SPACE_SHIP_ROTATION_STEP;
  else
    return;

  ss->pos->rotation += rads;

  if (isGreater(fabs(ss->pos->rotation), M_DOUBLE_PI))
    ss->pos->rotation = 0.0f;

  object_position_rotate_points(ss->pos, rads);
}

void space_ship_restart(SPACE_SHIP *ss) {
  if (!ss)
    return;
  ss->draw = true;
  ss->pos->rotation = ss->acceleration = 0.0f;
  _set_position(ss, ss->display.x/2.0f, ss->display.y/2.0f);
}

void space_ship_notify_keys(SPACE_SHIP *ss, KEYS pressed_keys) {
  if (!ss) {
    printf("Space ship is null!\n");
    return;
  }

  _space_ship_move(ss, pressed_keys);
  _space_ship_rotate(ss, pressed_keys);
  cannon_shoot(ss->c, ss->pos->center, ss->pos->rotation, pressed_keys);
}

unsigned space_ship_detect_collisions(SPACE_SHIP *ss,
				  OBJECT_POSITION *asteroids,
				  unsigned asteroids_size) {
  unsigned i, j;
  float minX, maxX, minY, maxY;

  if (!ss)
    return UINT_MAX;

  minX = ss->display.x * 2.0f;
  minY = ss->display.y * 2.0f;
  maxX = maxY = -100.0f;

  for (i = 0; i < ss->pos->points_size; i++) {
    minX = fminf(ss->pos->points[i].x, minX);
    minY = fminf(ss->pos->points[i].y, minY);
    maxX = fmaxf(ss->pos->points[i].x, maxX);
    maxY = fmaxf(ss->pos->points[i].y, maxY);
  }

  for (i = 0; i < asteroids_size; i++) {
    for (j = 0; j < asteroids[i].points_size; j++) {
      if (isGreater(asteroids[i].points[j].x, minX) && isLess(asteroids[i].points[j].x, maxX) &&
	  isGreater(asteroids[i].points[j].y, minY) && isLess(asteroids[i].points[j].y, maxY)) {
	ss->draw = false;
	return asteroids[i].id;
      }
    }
  }
  return UINT_MAX;
}

CANNON *space_ship_get_cannon(SPACE_SHIP *ss) {
  if (!ss)
    return NULL;
  return ss->c;
}

void space_ship_draw(SPACE_SHIP *ss) {
  if (!ss) {
    printf("Space ship is null!\n");
    return;
  }

  if (ss->draw)
    al_draw_filled_triangle(ss->pos->points[0].x,
			    ss->pos->points[0].y,
			    ss->pos->points[1].x,
			    ss->pos->points[1].y,
			    ss->pos->points[2].x,
			    ss->pos->points[2].y,
			    al_map_rgb(255, 255, 255));
  cannon_draw(ss->c);
}

void space_ship_destroy(SPACE_SHIP *ss) {
  if (!ss) {
    printf("Space ship is null!\n");
    return ;
  }
  cannon_destroy(ss->c);
  object_position_destroy(ss->pos);
  free(ss);
}
