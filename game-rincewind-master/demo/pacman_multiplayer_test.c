#include "pacman_multiplayer_test.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
  Gets the state of the frame containing the total time elapsed,
  an obj_list_t * of polygons in the frame, a vertex counter which
  will continually increase, and the color of the last polygon.
*/
typedef struct state {
  size_t time;
  scene_t *scene;
} state_t;

/*
  Tells which side a body is on or whether he is entirely in the frame
*/
typedef enum pacman_side { left, right, top, bottom, nah } pacman_side_t;

/**
 * @brief generates pellet and sets it to a random position
 * @param scene the current list of bodies on the screen
 */
void pellet_spawner(scene_t *scene) {
  body_t *new_pellet = body_init(list_init(PELL_VERTICES, (free_func_t)free),
                                 PELL_MASS, PELL_COLOR);

  // Generate pellet
  for (size_t i = 0; i < PELL_VERTICES; i++) {
    vector_t *vertex = malloc(sizeof(vector_t));
    *vertex = vec_rotate(PELL_INIT_VECTOR, i * 2 * M_PI / PELL_VERTICES);
    body_add_vertex(new_pellet, vertex);
  }

  // Shift pellet to random position
  vector_t position = {.x = rand() % WIN_WIDTH, .y = rand() % WIN_HEIGHT};
  body_set_centroid(new_pellet, position);

  scene_add_body(scene, new_pellet);
}

/**
 * @brief draws pacman and sets his initial position
 * @param scene the current list of bodies on the screen
 * @param radius the distance from the center of pacman's mouth to his edge
 */
void create_pacman(scene_t *scene, double radius, rgb_color_t color) {
  list_t *shape =
      list_init(SPHERE_VERTICES - MOUTH_VERTICES, (free_func_t)free);
  vector_t *center = malloc(sizeof(vector_t));
  *center = VEC_ZERO;
  list_add(shape, center);
  vector_t *first = malloc(sizeof(vector_t));
  *first = (vector_t){radius, 0};
  for (size_t i = MOUTH_VERTICES / 2; i < SPHERE_VERTICES - MOUTH_VERTICES / 2;
       i++) {
    vector_t *vertex = malloc(sizeof(vector_t));
    *vertex = vec_rotate(*first, ((double)(2 * M_PI)) * i / SPHERE_VERTICES);
    list_add(shape, vertex);
  }

  body_t *pacman = body_init(shape, DEFAULT_MASS, color);
  body_set_centroid(pacman, INIT_POSITION);
  scene_add_body(scene, pacman);
}

/**
 * @brief checks if a pellet has been eaten and removes it if it has
 * @param scene the current list of bodies on the screen
 */
void pellet_eaten(scene_t *scene) {
  body_t *pacman = scene_get_body(scene, 0);

  for (size_t i = 2; i < scene_bodies(scene); i++) {
    body_t *pellet = scene_get_body(scene, i);
    if (body_distance(pacman, pellet) <= COLLISION_RADIUS) {
      scene_remove_body(scene, i);
      break;
    }
  }
}

/**
 * @brief Checks if pacman is completely outside of bounds
 * @param scene the current list of bodies on the screen
 * @return side of screen pacman is out of or 'nah'
 */
pacman_side_t check_bounds_helper(scene_t *scene) {
  body_t *pacman = scene_get_body(scene, 0);
  bool out_right = true;
  bool out_left = true;
  bool out_top = true;
  bool out_bottom = true;
  for (size_t i = 0; i < list_size(body_get_shape(pacman)); i++) {
    double x_pos = ((vector_t *)list_get(body_get_shape(pacman), i))->x;
    double y_pos = ((vector_t *)list_get(body_get_shape(pacman), i))->y;
    if (x_pos <= WIN_WIDTH && out_right) {
      out_right = false;
    } else if (x_pos >= 0 && out_left) {
      out_left = false;
    } else if (y_pos <= WIN_HEIGHT + RADIUS && out_top) {
      out_top = false;
    } else if (y_pos >= -RADIUS && out_bottom) {
      out_bottom = false;
    }
  }
  if (out_right) {
    return right;
  }
  if (out_left) {
    return left;
  }
  if (out_top) {
    return top;
  }
  if (out_bottom) {
    return bottom;
  }
  return nah;
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
  body_t *pacman = scene_get_body(scene, 0);
  body_t *player_2 = scene_get_body(scene, 1);
  if (type == KEY_PRESSED) {
    switch (key) {
    case LEFT_ARROW:
      body_set_rotation(pacman, M_PI);
      if (body_get_velocity(pacman).x >= 0) {
        body_set_velocity(pacman, DEFAULT_LEFT_SPEED);
      } else {
        vector_t accelerated_vel_left = {
            body_get_velocity(pacman).x - ACCELERATION, 0};
        body_set_velocity(pacman, accelerated_vel_left);
      }
      break;
    case UP_ARROW:
      body_set_rotation(pacman, M_PI / 2.0);
      if (body_get_velocity(pacman).y <= 0) {
        body_set_velocity(pacman, DEFAULT_UP_SPEED);
      } else {
        vector_t accelerated_vel_up = {0, body_get_velocity(pacman).y +
                                              ACCELERATION};
        body_set_velocity(pacman, accelerated_vel_up);
      }
      break;

    case RIGHT_ARROW:
      body_set_rotation(pacman, 0);
      if (body_get_velocity(pacman).x <= 0) {
        body_set_velocity(pacman, DEFAULT_RIGHT_SPEED);
      } else {
        vector_t accelerated_vel_right = {
            body_get_velocity(pacman).x + ACCELERATION, 0};
        body_set_velocity(pacman, accelerated_vel_right);
      }
      break;
    case DOWN_ARROW:
      body_set_rotation(pacman, 3.0 * M_PI / 2);
      if (body_get_velocity(pacman).y >= 0) {
        body_set_velocity(pacman, DEFAULT_DOWN_SPEED);
      } else {
        vector_t accelerated_vel_down = {0, body_get_velocity(pacman).y -
                                                ACCELERATION};
        body_set_velocity(pacman, accelerated_vel_down);
      }
      break;

    case A_KEY:
      body_set_rotation(player_2, M_PI);
      if (body_get_velocity(player_2).x >= 0) {
        body_set_velocity(player_2, DEFAULT_LEFT_SPEED);
      } else {
        vector_t accelerated_vel_left = {
            body_get_velocity(player_2).x - ACCELERATION, 0};
        body_set_velocity(player_2, accelerated_vel_left);
      }
      break;
    case W_KEY:
      body_set_rotation(player_2, M_PI / 2.0);
      if (body_get_velocity(player_2).y <= 0) {
        body_set_velocity(player_2, DEFAULT_UP_SPEED);
      } else {
        vector_t accelerated_vel_up = {0, body_get_velocity(player_2).y +
                                              ACCELERATION};
        body_set_velocity(player_2, accelerated_vel_up);
      }
      break;

    case D_KEY:
      body_set_rotation(player_2, 0);
      if (body_get_velocity(player_2).x <= 0) {
        body_set_velocity(player_2, DEFAULT_RIGHT_SPEED);
      } else {
        vector_t accelerated_vel_right = {
            body_get_velocity(player_2).x + ACCELERATION, 0};
        body_set_velocity(player_2, accelerated_vel_right);
      }
      break;
    case S_KEY:
      body_set_rotation(player_2, 3.0 * M_PI / 2);
      if (body_get_velocity(player_2).y >= 0) {
        body_set_velocity(player_2, DEFAULT_DOWN_SPEED);
      } else {
        vector_t accelerated_vel_down = {0, body_get_velocity(player_2).y -
                                                ACCELERATION};
        body_set_velocity(player_2, accelerated_vel_down);
      }
      break;

    default:
      break;
    }
  }

  if (type == KEY_RELEASED) {

    if (key == UP_ARROW || key == DOWN_ARROW || key == RIGHT_ARROW ||
        key == LEFT_ARROW) {
      if (body_get_velocity(pacman).x > 0) {
        body_set_velocity(pacman, DEFAULT_RIGHT_SPEED);
      } else if (body_get_velocity(pacman).x < 0) {
        body_set_velocity(pacman, DEFAULT_LEFT_SPEED);
      } else if (body_get_velocity(pacman).y > 0) {
        body_set_velocity(pacman, DEFAULT_UP_SPEED);
      } else if (body_get_velocity(pacman).y < 0) {
        body_set_velocity(pacman, DEFAULT_DOWN_SPEED);
      }
    } else if (key == W_KEY || key == A_KEY || key == S_KEY || key == D_KEY) {
      if (body_get_velocity(player_2).x > 0) {
        body_set_velocity(player_2, DEFAULT_RIGHT_SPEED);
      } else if (body_get_velocity(player_2).x < 0) {
        body_set_velocity(player_2, DEFAULT_LEFT_SPEED);
      } else if (body_get_velocity(player_2).y > 0) {
        body_set_velocity(player_2, DEFAULT_UP_SPEED);
      } else if (body_get_velocity(player_2).y < 0) {
        body_set_velocity(player_2, DEFAULT_DOWN_SPEED);
      }
    }
  }
}

/**
 * @brief wraps pacman to the opposing side if he hits an edge
 * @param scene the current list of bodies on the screen
 */
void wrap(scene_t *scene) {
  body_t *pacman = scene_get_body(scene, 0);

  pacman_side_t out_of_bounds = check_bounds_helper(scene);

  if (out_of_bounds == left) {
    vector_t right_edge = {WIN_WIDTH, body_get_centroid(pacman).y};
    vector_t radius = {RADIUS, 0};
    vector_t spawn = vec_add(right_edge, radius);
    body_set_centroid(pacman, spawn);
  }

  else if (out_of_bounds == top) {
    vector_t bottom_edge = {body_get_centroid(pacman).x, 0};
    vector_t radius = {0, RADIUS};
    vector_t spawn = vec_subtract(bottom_edge, radius);
    body_set_centroid(pacman, spawn);
  }

  else if (out_of_bounds == right) {
    vector_t edge = {0, body_get_centroid(pacman).y};
    vector_t radius = {RADIUS, 0};
    vector_t spawn = vec_subtract(edge, radius);
    body_set_centroid(pacman, spawn);
  }

  else if (out_of_bounds == bottom) {
    vector_t edge = {body_get_centroid(pacman).x, WIN_HEIGHT};
    vector_t radius = {0, RADIUS};
    vector_t spawn = vec_add(edge, radius);
    body_set_centroid(pacman, spawn);
  }
}

/**
 * @brief Initialise and allocate memory for state.
 * Intialise scene, pacman, first set of pellets, and
 * time.
 * @return the current state
 */
state_t *emscripten_init() {

  state_t *state = malloc(sizeof(state_t));

  sdl_init(VEC_ZERO, (vector_t){WIN_WIDTH, WIN_HEIGHT});
  scene_t *scene = scene_init();
  create_pacman(scene, RADIUS, PACMAN_COLOR);
  create_pacman(scene, RADIUS, PACMAN_2_COLOR);

  state->time = 0.0;
  state->scene = scene;

  for (size_t i = 0; i < PELLET_START; i++) {
    pellet_spawner(scene);
  }

  sdl_on_key(key_event_handler);
  return state;
}

/**
 * @brief Increments time and draws each frame
 * @param state state_t describing current frame
 */
void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  if (dt > 0) {
    state->time += 1;
    if (state->time > PELL_SPAWN_RATE) {
      pellet_spawner(state->scene);
      state->time = 0;
    }
  }
  wrap(state->scene);
  pellet_eaten(state->scene);
  pellet_eaten(state->scene);
  scene_tick(state->scene, 50 * dt);
  sdl_render_scene(state->scene);
}

/**
 * @brief Frees all polygons from polygon list, then frees polygon list itself
 * finally frees the entire state
 * @param state state_t describing the current frame
 */
void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}