#include "space_invaders.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
  Gets the state of the frame containing the total time elapsed,
  an obj_list_t * of polygons in the frame, a vertex counter which
  will continually increase, and the color of the last polygon.
*/
typedef struct state {
  double time_since_last_shot;
  scene_t *scene;
} state_t;

/*
  Tells type of a body
*/
typedef enum type {
  player_type,
  player_bullet_type,
  invader_type,
  invader_bullet_type
} type_t;

/*
  Tells which side a body is on or whether he is entirely in the frame
*/
typedef enum side_out { left, right, top, bottom, none } side_out;

void shot_bind(scene_t *scene, body_t *bullet) {
  type_t type = *(type_t *)body_get_info(bullet);
  if (type == player_bullet_type) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
      body_t *body = scene_get_body(scene, i);
      if (*(type_t *)body_get_info(body) == invader_type &&
          !body_is_removed(body)) {
        create_destructive_collision(scene, bullet, body, true, true);
      }
    }
  } else if (type == invader_bullet_type) {
    body_t *player = scene_get_body(scene, 0);
    create_destructive_collision(scene, bullet, player, true, true);
  }
}

/**
 * @brief Create a bullet object that is a rectangle
 *
 * @param scene
 * @param init_position
 */
void create_shot(scene_t *scene, vector_t init_position, type_t source) {
  list_t *shape = list_init(4, (free_func_t)free);
  vector_t *top_left = malloc(sizeof(vector_t));
  vector_t *top_right = malloc(sizeof(vector_t));
  vector_t *bot_left = malloc(sizeof(vector_t));
  vector_t *bot_right = malloc(sizeof(vector_t));

  *top_left = vec_add(init_position, (vector_t){-B_WIDTH, B_HEIGHT});
  *top_right = vec_add(init_position, (vector_t){B_WIDTH, B_HEIGHT});
  *bot_left = vec_add(init_position, (vector_t){-B_WIDTH, -B_HEIGHT});
  *bot_right = vec_add(init_position, (vector_t){B_WIDTH, -B_HEIGHT});
  list_add(shape, top_left);
  list_add(shape, bot_left);
  list_add(shape, bot_right);
  list_add(shape, top_right);

  type_t *type = malloc(sizeof(type_t));
  vector_t velocity = VEC_ZERO;
  rgb_color_t color = {0, 0, 0};
  if (source == player_type) {
    *type = player_bullet_type;
    velocity = P_B_VELOCITY;
    color = P_B_COLOR;
  } else if (source == invader_type) {
    *type = invader_bullet_type;
    velocity = INVDR_B_VELOCITY;
    color = INVDR_B_COLOR;
  }
  body_t *bullet = body_init_with_info(shape, B_MASS, color, type, free);
  body_set_velocity(bullet, velocity);
  scene_add_body(scene, bullet);

  shot_bind(scene, bullet);
}

void create_invaders(scene_t *scene, double invdr_radius, double invdr_arc_mult,
                     rgb_color_t invdr_color, double init_spawn_height,
                     size_t num_rows_invdrs) {
  size_t invdrs_per_row = WIN_WIDTH / (2 * invdr_radius + INVDR_BUFFER);
  for (size_t i = 0; i < num_rows_invdrs; i++) {
    for (size_t j = 0; j < invdrs_per_row; j++) {
      list_t *shape =
          list_init(INVDR_ARC_MULT * INVDR_VERTICES, (free_func_t)free);
      vector_t *first = malloc(sizeof(vector_t));
      *first = (vector_t){0, invdr_radius};
      list_add(shape, first);

      // Adding in counterclockwise
      for (size_t k = 1; k < INVDR_ARC_MULT * INVDR_VERTICES / 2; k++) {
        vector_t *vertex_left = malloc(sizeof(vector_t));
        *vertex_left = vec_rotate(*first, (2 * M_PI) * k / INVDR_VERTICES);
        list_add(shape, vertex_left);
      }
      vector_t *center = malloc(sizeof(vector_t));
      *center = VEC_ZERO;
      list_add(shape, center);
      for (size_t k = INVDR_ARC_MULT * INVDR_VERTICES / 2; k > 0; k--) {
        vector_t *vertex_right = malloc(sizeof(vector_t));
        *vertex_right = vec_rotate(*first, -(2 * M_PI) * k / INVDR_VERTICES);
        list_add(shape, vertex_right);
      }

      type_t *type = malloc(sizeof(type_t));
      *type = invader_type;
      body_t *invader =
          body_init_with_info(shape, INVDR_MASS, INVDR_COLOR, type, free);
      body_set_centroid(invader,
                        (vector_t){(j + 1) * (2 * invdr_radius + INVDR_BUFFER),
                                   INVDR_INIT_SPAWN_HEIGHT -
                                       i * (invdr_radius + INVDR_BUFFER)});
      body_set_velocity(invader, (vector_t){INVDR_SPEED, 0});
      scene_add_body(scene, invader);
    }
  }
}

/**
 * @brief draws player and sets his initial position
 * @param scene the current list of bodies on the screen
 * @param radius the distance from the center of pacman's mouth to his edge
 */
void create_player(scene_t *scene, double p_height, double p_width,
                   rgb_color_t p_color) {
  list_t *shape = list_init(P_VERTICES, (free_func_t)free);
  for (size_t i = 0; i < P_VERTICES; i++) {
    vector_t *vertex = malloc(sizeof(vector_t));
    *vertex = (vector_t){p_width * cos(2 * M_PI * (i / P_VERTICES)),
                         p_height * sin(2 * M_PI * (i / P_VERTICES))};
    list_add(shape, vertex);
  }

  type_t *type = malloc(sizeof(type_t));
  *type = player_type;
  body_t *player = body_init_with_info(shape, P_MASS, p_color, type, free);
  body_set_centroid(player, (vector_t){WIN_WIDTH / 2, p_height});
  scene_add_body(scene, player);
}

/**
 * @brief Checks if body is completely outside of bounds
 * @param scene the current list of bodies on the screen
 * @return side of screen pacman is out of or 'nah'
 */
side_out check_bounds_helper(body_t *body) {
  type_t type = *(type_t *)body_get_info(body);
  double radius = 0;
  switch (type) {
  case player_type:
    radius = P_WIDTH;
    break;
  case invader_type:
    radius = INVDR_RADIUS;
    break;
  case player_bullet_type:
    if (body_get_centroid(body).y > WIN_HEIGHT) {
      body_remove(body);
    }
    return top;
  case invader_bullet_type:
    if (body_get_centroid(body).y < 0) {
      body_remove(body);
    }
    return bottom;
  default:
    break;
  }

  if (body_get_centroid(body).x + radius > WIN_WIDTH) {
    return right;
  } else if (body_get_centroid(body).x - radius < 0) {
    return left;
  }
  return none;
}

void check_player_bounds(body_t *player) {
  side_out side = check_bounds_helper(player);
  vector_t centroid = body_get_centroid(player);
  switch (side) {
  case right:
    body_set_centroid(player, (vector_t){WIN_WIDTH - P_WIDTH, centroid.y});
    break;
  case left:
    body_set_centroid(player, (vector_t){P_WIDTH, centroid.y});
    break;
  default:
    break;
  }
}

bool no_player_bullet_in_scene(scene_t *scene) {
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    type_t type = *(type_t *)body_get_info(body);
    // Because the player and invaders are first in the list,
    // If we see one of them, we know there must not be a bullet on screen
    if (type == player_bullet_type && !body_is_removed(body)) {
      return false;
    }
  }
  return true;
}

/**
 * @brief does the appropriate transformations to pacman when a certain key is
 * pressed
 * @param state state_t describing current frame
 * @param key the key being pressed
 * @param type whether key is pressed or not pressed
 * @param held_time the amountt of time the key is held for
 */
void key_event_handler(char key, key_event_type_t type, double held_time,
                       state_t *state) {
  scene_t *scene = state->scene;
  body_t *player = scene_get_body(scene, 0);
  if (type == KEY_PRESSED) {
    switch (key) {
    case SPACE:
      if (no_player_bullet_in_scene(state->scene)) {
        create_shot(state->scene, body_get_centroid(player), player_type);
      }
      break;
    case LEFT_ARROW:
      body_set_velocity(player, (vector_t){-P_SPEED, 0});
      break;
    case RIGHT_ARROW:
      body_set_velocity(player, (vector_t){P_SPEED, 0});
      break;

    default:
      break;
    }
  }
  if (type == KEY_RELEASED) {
    if (key != SPACE) {
      body_set_velocity(player, VEC_ZERO);
    }
  }
}

void invader_shift(scene_t *scene) {
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    if (*(type_t *)body_get_info(body) == invader_type) {
      side_out side = check_bounds_helper(body);
      double x_shift = 0;
      vector_t new_centroid = VEC_ZERO;
      switch (side) {
      case right:
        body_set_velocity(body, (vector_t){-INVDR_SPEED, 0});
        x_shift = -INVDR_BUFFER;
        new_centroid.x = WIN_WIDTH - INVDR_RADIUS;
        break;
      case left:
        body_set_velocity(body, (vector_t){INVDR_SPEED, 0});
        x_shift = INVDR_BUFFER;
        new_centroid.x = INVDR_RADIUS;
        break;
      default:
        break;
      }
      if (side != none) {
        new_centroid.y = body_get_centroid(body).y;
        vector_t shift = {x_shift, INVDR_NUM_OF_ROWS * -1.0 *
                                       (INVDR_RADIUS + INVDR_BUFFER)};
        body_set_centroid(body, vec_add(new_centroid, shift));
      }
    }
  }
}

void check_game_end(scene_t *scene) {
  // if the player has been hit
  if (*(type_t *)body_get_info(scene_get_body(scene, 0)) != player_type) {
    printf("You Lose! You've Been Hit!\n");
    exit(0);
  }

  // if there are enemies
  bool enemies_left = false;
  for (int i = 0; i < scene_bodies(scene); i++) {
    if (*(type_t *)body_get_info(scene_get_body(scene, i)) == invader_type) {
      enemies_left = true;
    }
  }
  if (!enemies_left) {
    printf("You Win! Congratulations!\n");
    exit(0);
  }

  // if enemies have reached bottom
  for (int i = 0; i < scene_bodies(scene); i++) {
    if (*(type_t *)body_get_info(scene_get_body(scene, i)) == invader_type &&
        body_get_centroid(scene_get_body(scene, i)).y < LOSE_HEIGHT) {
      printf("You Lose! They Reached your base!\n");
      exit(0);
    }
  }
}

void game_move(state_t *state, double dt) {
  check_game_end(state->scene);

  // Checking bounds
  invader_shift(state->scene);
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    type_t type = *(type_t *)body_get_info(body);
    if (type == player_type) {
      check_player_bounds(body);
    } else if (type == player_bullet_type) {
      check_bounds_helper(body);
    } else if (type == invader_bullet_type) {
      check_bounds_helper(body);
    }
  }

  // Creating enemy bullet
  if (state->time_since_last_shot > 40) {
    size_t ind = -1;
    do {
      ind = rand() % scene_bodies(state->scene);
    } while (*(type_t *)body_get_info(scene_get_body(state->scene, ind)) !=
             invader_type);

    create_shot(state->scene,
                  body_get_centroid(scene_get_body(state->scene, ind)),
                  invader_type);
    state->time_since_last_shot = 0;
  }

  scene_tick(state->scene, TIME_MULT * dt);
}

state_t *emscripten_init(void) {
  sdl_init(VEC_ZERO, (vector_t){WIN_WIDTH, WIN_HEIGHT});

  state_t *state = malloc(sizeof(state_t));
  state->time_since_last_shot = 0;
  state->scene = scene_init();
  create_player(state->scene, P_HEIGHT, P_WIDTH, P_COLOR);
  create_invaders(state->scene, INVDR_RADIUS, INVDR_ARC_MULT, INVDR_COLOR,
                  INVDR_INIT_SPAWN_HEIGHT, INVDR_NUM_OF_ROWS);

  sdl_on_key(key_event_handler);
  sdl_render_scene(state->scene);
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  state->time_since_last_shot += TIME_MULT * dt;
  game_move(state, dt);
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) { scene_free(state->scene); }