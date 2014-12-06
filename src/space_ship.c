#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "float_compare.h"
#include "space_ship.h"
#include "cannon.h"

//This height should be scaled.
#define SPACE_SHIP_HEIGHT (24.0f)
#define SPACE_SHIP_BASE_WIDTH (10.0f)
#define SPACE_SHIP_HALF_HEIGHT ((SPACE_SHIP_HEIGHT)/2.0f)

#define SPACE_SHIP_ROTATION_STEP (0.104719755f)
#define MAX_ACCELERATION (10.0f)
#define ACCELIERATION_STEP (0.02f)

struct _SPACE_SHIP {
  /* Where the tip of the space ship is pointing */
  POINT tip;

  POINT center;

  POINT left_p;
  POINT right_p;

  POINT display;

  float rotation_degrees;
  float acceleration;

  CANNON *c;
};

static void _rotate_space_ship(SPACE_SHIP *ss, float degrees) {
  ss->tip = rotate_point(ss->tip, ss->center, degrees);
  ss->left_p = rotate_point(ss->left_p, ss->center, degrees);
  ss->right_p = rotate_point(ss->right_p, ss->center, degrees);
}

static void _set_position(SPACE_SHIP *ss, float x, float y) {

  if (isGreater(y, ss->display.y))
    ss->center.y = 0.0f;
  else if (isLess(y, 0.0))
    ss->center.y = ss->display.y;
  else
    ss->center.y = y;

  if (isGreater(x, ss->display.x))
    ss->center.x = 0.0f;
  else if (isLess(x, 0.0))
    ss->center.x = ss->display.x;
  else
    ss->center.x = x;

  ss->tip.x = ss->center.x;
  ss->tip.y = ss->center.y - SPACE_SHIP_HALF_HEIGHT;

  ss->left_p.x = ss->center.x - SPACE_SHIP_BASE_WIDTH;
  ss->left_p.y = ss->center.y + SPACE_SHIP_HALF_HEIGHT;

  ss->right_p.x = ss->center.x + SPACE_SHIP_BASE_WIDTH;
  ss->right_p.y = ss->center.y + SPACE_SHIP_HALF_HEIGHT;

  _rotate_space_ship(ss, ss->rotation_degrees);
}

SPACE_SHIP *space_ship_create(POINT display) {
  SPACE_SHIP *ss = calloc(1, sizeof(SPACE_SHIP));
  if (!ss)
    return NULL;

  ss->display = display;

  _set_position(ss, display.x/2.0f, display.y/2.0f);

  ss->c = cannon_create(ss->display);
  if (!ss->c) {
    free(ss);
    return NULL;
  }

  return ss;
}

static void _space_ship_move(SPACE_SHIP *ss, KEYS pressed_keys) {
  POINT p;
  bool automaticStopDown = false;
  bool automaticStopUp = false;

  p = ss->center;

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

  p.x += sinf(ss->rotation_degrees) * ss->acceleration;
  p.y -= cosf(ss->rotation_degrees) * ss->acceleration;

  if (!isEqual(p.x, ss->center.x) || !isEqual(p.y, ss->center.y))
    _set_position(ss, p.x, p.y);
}

static void _space_ship_rotate(SPACE_SHIP *ss, KEYS pressed_keys) {
  float degrees;

  if (pressed_keys & LEFT)
    degrees = -SPACE_SHIP_ROTATION_STEP;
  else if (pressed_keys & RIGHT)
    degrees = SPACE_SHIP_ROTATION_STEP;
  else
    return;

  ss->rotation_degrees += degrees;

  if (isGreater(fabs(ss->rotation_degrees), M_DOUBLE_PI))
    ss->rotation_degrees = 0.0f;

  _rotate_space_ship(ss, degrees);
}

void space_ship_notify_keys(SPACE_SHIP *ss, KEYS pressed_keys) {
  if (!ss) {
    printf("Space ship is null!\n");
    return;
  }
  _space_ship_move(ss, pressed_keys);
  _space_ship_rotate(ss, pressed_keys);
  cannon_shoot(ss->c, ss->center, ss->rotation_degrees, pressed_keys);
}

void space_ship_draw(SPACE_SHIP *ss) {
  if (!ss) {
    printf("Space ship is null!\n");
    return;
  }

  al_draw_filled_triangle(ss->tip.x,
			  ss->tip.y,
			  ss->left_p.x,
			  ss->left_p.y,
			  ss->right_p.x,
			  ss->right_p.y,
			  al_map_rgb(255, 255, 255));
  cannon_draw(ss->c);
}

void space_ship_destroy(SPACE_SHIP *ss) {
  if (!ss) {
    printf("Space ship is null!\n");
    return ;
  }
  cannon_destroy(ss->c);
  free(ss);
}
