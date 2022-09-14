#include "forces.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CIRCLE_POINTS 40

#define MAX ((vector_t){.x = 100.0, .y = 50.0})

#define N_ROWS 3
#define N_COLS 10

#define PLAYER_HEIGHT 2
#define PLAYER_WIDTH 10
#define PLAYER_WIDTH1 2 * PLAYER_WIDTH
#define PLAYER_WIDTH2 3 * PLAYER_WIDTH
#define PLAYER_SPEED 80
#define BRICKS_REMAINING1 20
#define BRICKS_REMAINING2 10

#define BRICK_BUFFER 1
#define BRICK_LENGTH (MAX.x / N_COLS - BRICK_BUFFER)
#define BRICK_HEIGHT 3
#define BALL_RADIUS 1.0
#define BALL_ELASTICITY 0.7
#define BALL_DX 5
#define BALL_DY 5
#define WALL_WIDTH 1.0
#define DELTA_X 1.0
#define DROP_Y (MAX.y - 3.0)

#define BALL_MASS 2.0
#define BALL_START_POS ((vector_t){MAX.x / 2, PLAYER_HEIGHT + 10})
#define BALL_START_VEL ((vector_t){30.0, 30.0})
#define BALL_COLOR ((rgb_color_t){1, 0, 0})
#define BRICK_START_COLOR ((rgb_color_t){0, 1, 0})
#define WALL_COLOR ((rgb_color_t){0, 0, 1})
#define PLAYER_COLOR ((rgb_color_t){1, 1, 0})

typedef struct state {
  scene_t *scene;
  bool first_powerup;
  bool second_powerup;
} state_t;

typedef enum type {
  BALL,
  BRICK,
  WALLS // or player
} type_t;

type_t *make_type_info(type_t type) {
  type_t *info = malloc(sizeof(*info));
  *info = type;
  return info;
}

type_t get_type(body_t *body) {
  return *(type_t *)body_get_info(body);
}

/** Creates a ball with the given starting position and velocity */
body_t *get_ball(vector_t center, vector_t velocity) {
  list_t *shape = circle_init(BALL_RADIUS, CIRCLE_POINTS);
  body_t *ball = body_init_with_info(shape, BALL_MASS, BALL_COLOR,
                                     make_type_info(BALL), free);

  body_set_centroid(ball, center);
  body_set_velocity(ball, velocity);

  return ball;
}

void make_player(scene_t *scene) {
  list_t *rect = rect_init(PLAYER_WIDTH, PLAYER_HEIGHT);
  body_t *player = body_init_with_info(rect, INFINITY, PLAYER_COLOR,
                                       make_type_info(WALLS), free);
  body_set_centroid(player, (vector_t){MAX.x / 2, PLAYER_HEIGHT / 2});
  scene_add_body(scene, player);
}

/** Adds a ball to the scene */
void add_ball(scene_t *scene) {
  // Add the ball to the scene.
  vector_t ball_center =
      vec_add(BALL_START_POS, (vector_t){rand() % BALL_DX, rand() % BALL_DY});
  body_t *ball = get_ball(ball_center, BALL_START_VEL);
  size_t body_count = scene_bodies(scene);
  scene_add_body(scene, ball);

  // Add force creators with other bodies
  for (size_t i = 0; i < body_count; i++) {
    body_t *body = scene_get_body(scene, i);
    switch (get_type(body)) {
    case WALLS:
      // Bounce off walls and bricks
      create_physics_collision(scene, 1, ball, body);
      break;
    case BRICK:
      create_physics_collision(scene, 1, ball, body);
      create_destructive_collision(scene, ball, body, false, true);
      break;
    case BALL:
      break;
    }
  }
}

/** Adds the pegs to the scene */
void add_bricks(scene_t *scene) {
  // Add N_ROWS and N_COLS of pegs.
  for (size_t i = 0; i < N_ROWS; i++) {
    for (size_t j = 0; j < N_COLS; j++) {
      list_t *rect = rect_init(BRICK_LENGTH, BRICK_HEIGHT);
      rgb_color_t rand_color = {(rand() % 255) / 255.0, (rand() % 255) / 255.0,
                                (rand() % 255) / 255.0};
      body_t *body = body_init_with_info(rect, INFINITY, rand_color,
                                         make_type_info(BRICK), free);
      body_set_centroid(body,
                        (vector_t){(j * (BRICK_LENGTH + BRICK_BUFFER)) +
                                       (BRICK_LENGTH / 2) + BRICK_BUFFER,
                                   MAX.y - (i * (BRICK_HEIGHT + BRICK_BUFFER)) -
                                       ((BRICK_HEIGHT / 2) + BRICK_BUFFER)});
      scene_add_body(scene, body);
    }
  }
}

/** Adds the walls to the scene */
void add_walls(scene_t *scene) {
  // Add left wall
  list_t *rect = rect_init(WALL_WIDTH, MAX.y);
  body_t *left = body_init_with_info(rect, INFINITY, WALL_COLOR,
                                     make_type_info(WALLS), free);
  body_set_centroid(left, (vector_t){0, MAX.y / 2});
  scene_add_body(scene, left);
  // add right wall
  rect = rect_init(WALL_WIDTH, MAX.y);
  body_t *right = body_init_with_info(rect, INFINITY, WALL_COLOR,
                                      make_type_info(WALLS), free);
  body_set_centroid(right, (vector_t){MAX.x, MAX.y / 2});
  scene_add_body(scene, right);

  // add ceiling
  rect = rect_init(MAX.x, WALL_WIDTH);
  body_t *ceiling = body_init_with_info(rect, INFINITY, WALL_COLOR,
                                        make_type_info(WALLS), free);
  body_set_centroid(ceiling,
                    (vector_t){.x = MAX.x / 2, .y = MAX.y - WALL_WIDTH / 2});
  scene_add_body(scene, ceiling);
}

void key_event_handler(char key, key_event_type_t type, double held_time,
                       state_t *state) {
  scene_t *scene = state->scene;
  body_t *player = scene_get_body(scene, 0);
  if (type == KEY_PRESSED) {
    switch (key) {
    case LEFT_ARROW:
      body_set_velocity(player, (vector_t){-PLAYER_SPEED, 0});
      break;
    case RIGHT_ARROW:
      body_set_velocity(player, (vector_t){PLAYER_SPEED, 0});
      break;

    default:
      break;
    }
  }
  if (type == KEY_RELEASED) {
    body_set_velocity(player, VEC_ZERO);
  }
}

void check_player_bounds(body_t *player, size_t bricks_remaining) {
  vector_t centroid = body_get_centroid(player);
  double width = PLAYER_WIDTH;
  if (bricks_remaining < BRICKS_REMAINING2) {
    width = PLAYER_WIDTH2;
  } else if (bricks_remaining < BRICKS_REMAINING1) {
    width = PLAYER_WIDTH1;
  }
  if (body_get_centroid(player).x + width / 2 > MAX.x) {
    body_set_centroid(player, (vector_t){MAX.x - width / 2, centroid.y});
  } else if (body_get_centroid(player).x - width / 2 < 0) {
    body_set_centroid(player, (vector_t){width / 2, centroid.y});
  }
}

bool check_ball_outbounds(scene_t *scene) {
  body_t *ball = scene_get_body(scene, scene_bodies(scene) - 1);
  // player 0th index, ball is 1st index
  return body_get_centroid(ball).y <= 0;
}

void game_reset(state_t *state) {
  scene_free(state->scene);
  scene_t *scene = scene_init();
  // Add elements to the scene
  make_player(scene);
  add_bricks(scene);
  add_walls(scene);
  add_ball(scene);
  // Repeatedly render scene

  state->scene = scene;
}

state_t *emscripten_init(void) {
  // Initialize scene
  srand(time(NULL));
  sdl_init(VEC_ZERO, MAX);
  scene_t *scene = scene_init();

  // Add elements to the scene
  make_player(scene);
  add_bricks(scene);
  add_walls(scene);
  add_ball(scene);

  state_t *state = malloc(sizeof(state_t));
  state->scene = scene;
  state->first_powerup = false;
  state->second_powerup = false;
  sdl_on_key(key_event_handler);
  sdl_render_scene(state->scene);
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  check_player_bounds(scene_get_body(state->scene, 0),
                      scene_bodies(state->scene) - 5);

  if (scene_bodies(state->scene) - 5 < BRICKS_REMAINING1 &&
      !state->first_powerup) {
    // increase size
    state->first_powerup = true;
    list_t *rect = rect_init(PLAYER_WIDTH1, PLAYER_HEIGHT);
    vector_t centroid = body_get_centroid(scene_get_body(state->scene, 0));
    body_set_shape(scene_get_body(state->scene, 0), rect);
    body_set_centroid(scene_get_body(state->scene, 0), centroid);
  }
  if (scene_bodies(state->scene) - 5 < BRICKS_REMAINING2 &&
      !state->second_powerup) {
    // increase size again
    state->second_powerup = true;
    list_t *rect = rect_init(PLAYER_WIDTH2, PLAYER_HEIGHT);
    vector_t centroid = body_get_centroid(scene_get_body(state->scene, 0));
    body_set_shape(scene_get_body(state->scene, 0), rect);
    body_set_centroid(scene_get_body(state->scene, 0), centroid);
  }

  if (check_ball_outbounds(state->scene) || scene_bodies(state->scene) == 5) {
    // clear everything
    game_reset(state);
  }

  scene_tick(state->scene, dt);
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}
