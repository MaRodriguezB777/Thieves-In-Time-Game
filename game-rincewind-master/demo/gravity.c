#include "gravity.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
  Gets the state of the frame containing the total time elapsed,
  an obj_list_t * of polygons in the frame, a vertex counter which
  will continually increase, and the color of the last polygon.
*/
typedef struct state {
  size_t time;
  list_t *polygon_list;
  int vertex_counter;
  rgb_color_t last_color;
} state_t;

/**
 * @brief Create a star shape object
 *
 * @param vertices
 * @param radius
 * @param inner_radius_mult
 * @param init_position
 * @return list_t* shape of star to be passed into body_t*
 */
list_t *create_star_shape(size_t vertices, double radius,
                          double inner_radius_mult) {
  list_t *star_shape = list_init(vertices, (free_func_t)free);

  vector_t *reference = malloc(sizeof(vector_t));
  *reference = (vector_t){radius, 0};
  list_add(star_shape, reference);

  for (size_t i = 1; i < vertices; i++) {
    vector_t *vertex = malloc(sizeof(vector_t));
    *vertex = vec_rotate(*reference, i * M_PI * 2 / vertices);

    if (i % 2 == 1) {
      *vertex = vec_multiply(inner_radius_mult, *vertex);
    }
    list_add(star_shape, vertex);
  }

  return star_shape;
}

/**
 * @brief Changes the speed of poly, applying gravity
 *
 * @param poly polygon to update
 * @param time time since last frame
 */
void apply_gravity(body_t *poly, double time) {
  list_t *shape = body_get_shape(poly);
  for (size_t i = 0; i < list_size(shape); i++) {
    vector_t org_speed = body_get_velocity(poly);
    vector_t new_speed = vec_add(org_speed, vec_multiply(time, GRAVITY_VEC));
    body_set_velocity(poly, new_speed);
  }
}

/**
 * @brief Checks that the bounds are
 *
 * @param poly
 * @return true
 * @return false
 */
bool check_bounds_helper(body_t *body) {
  bool all_pts_out = true;
  list_t *shape = body_get_shape(body);
  size_t vertices = list_size(shape);
  for (size_t i = 0; i < vertices; i++) {
    double x_pos = ((vector_t *)list_get(shape, i))->x;
    double y_pos = ((vector_t *)list_get(shape, i))->y;
    if (x_pos <= WIN_WIDTH) {
      all_pts_out = false;
    }

    if ((y_pos <= 0 && body_get_velocity(body).y < 0) ||
        (y_pos >= WIN_HEIGHT && body_get_velocity(body).y > 0)) {
      vector_t new_speed = {body_get_velocity(body).x,
                            body_get_velocity(body).y * -ELASTICITY_MULT};
      body_set_velocity(body, new_speed);
    }
  }
  return all_pts_out;
}

/**
 * @brief Translate and rotate polygon according to the amount of time that has
 * passed
 * @param polygon_list list of body_t to move
 * @param time time since last frame
 */
void move_objects_helper(list_t *polygon_list, double time) {
  for (size_t i = 0; i < list_size(polygon_list); i++) {
    body_t *poly = ((body_t *)list_get(polygon_list, i));
    if (check_bounds_helper(poly)) {
      free(list_remove(polygon_list, i));
      continue;
    }
    body_set_centroid(poly,
                      vec_add(body_get_centroid(poly),
                              vec_multiply(time, body_get_velocity(poly))));
    body_rotate(poly, body_get_rot_velocity(poly));
    apply_gravity(poly, time);
  }
}

/**
 * @brief Draws all the polygons in the frame
 *
 * @param state state_t describing the frame
 */
void sdl_draw_all(state_t *state) {
  for (size_t i = 0; i < list_size(state->polygon_list); i++) {
    body_t *poly = (body_t *)list_get(state->polygon_list, i);
    float red = ((size_t)body_get_color(poly).r % 255) / 255.0;
    float green = ((size_t)body_get_color(poly).g % 255) / 255.0;
    float blue = ((size_t)body_get_color(poly).b % 255) / 255.0;
    if (red > 1) {
      red = 1;
    } else if (red < 0) {
      red = 0;
    }
    if (blue > 1) {
      blue = 1;
    } else if (blue < 0) {
      blue = 0;
    }
    if (green > 1) {
      green = 1;
    } else if (green < 0) {
      green = 0;
    }
    sdl_draw_polygon(body_get_shape(poly), (rgb_color_t){red, green, blue});
  }
}

/**
 * @brief Initialise and allocate memory for state.
 * Intialise polygon_list, the first star in polygon_list
 * time and vertex counter.
 */
state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  state->polygon_list = list_init(300, (free_func_t)body_free);

  sdl_init(VEC_ZERO, (vector_t){WIN_WIDTH, WIN_HEIGHT});
  list_t *first_star_shape =
      create_star_shape(2 * STAR_VERTICES, STAR_RADIUS, INNER_RADIUS_MULT);
  body_t *first_star = body_init(first_star_shape, DEFAULT_MASS, BLACK);
  body_set_velocity(first_star, INIT_TRANS_SPEED);
  body_set_rot_velocity(first_star, INIT_ROT_SPEED);
  body_set_centroid(first_star, INIT_POSITION);
  state->time = 0;
  state->vertex_counter = STAR_VERTICES;
  list_add(state->polygon_list, (void *)first_star);
  return state;
}

/**
 * @brief Increments time and draws each frame
 * @param state state_t describing current frame
 */
void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  state->time += 1;

  if (state->time % 140 < 1) {
    state->vertex_counter += 1;
    state->last_color.r += 70.0;
    state->last_color.g += 100.0;
    state->last_color.b += 120.0;
    body_t *new_poly =
        body_init(create_star_shape(state->vertex_counter * 2, STAR_RADIUS,
                                    INNER_RADIUS_MULT),
                  DEFAULT_MASS, state->last_color);
    body_set_velocity(new_poly, INIT_TRANS_SPEED);
    body_set_rot_velocity(new_poly, INIT_ROT_SPEED);
    body_set_centroid(new_poly, INIT_POSITION);
    list_add(state->polygon_list, (void *)new_poly);
  }

  move_objects_helper(state->polygon_list, dt);
  sdl_clear();
  sdl_draw_all(state);
  sdl_show();
}

/**
 * @brief Fress all polygons from polygon list, then frees polygon list itself
 * finally frees the entire state
 * @param state state_t describing the current frame
 */
void emscripten_free(state_t *state) {
  for (size_t i = 0; i < list_size(state->polygon_list); i++) {
    body_free(list_get(state->polygon_list, i));
  }
  list_free(state->polygon_list);
  free(state);
}