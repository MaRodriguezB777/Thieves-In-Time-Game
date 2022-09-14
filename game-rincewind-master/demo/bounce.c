#include "bounce.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct state {
  list_t *polygon;
  vector_t trans_speed;
  double rot_speed;
  double radius;
} state_t;

/*
  For each vertex_location (x,y):
    if x >= width or x <= 0:
      reverse speed.x
    if y >= height or y <= 0:
      reverse speed.y
  */
void detect_wall_helper(state_t *state) {
  for (size_t i = 0;
       i < list_size(body_get_shape((body_t *)list_get(state->polygon, 0)));
       i++) {
    double x_pos =
        ((vector_t *)list_get(
             body_get_shape((body_t *)list_get(state->polygon, 0)), i))
            ->x;
    double y_pos =
        ((vector_t *)list_get(
             body_get_shape((body_t *)list_get(state->polygon, 0)), i))
            ->y;
    if (x_pos >= WIN_WIDTH || x_pos <= 0) {
      state->trans_speed.x = -state->trans_speed.x;
    }
    if (y_pos >= WIN_HEIGHT || y_pos <= 0) {
      state->trans_speed.y = -state->trans_speed.y;
    }

    body_set_velocity((body_t *)list_get(state->polygon, 0),
                      state->trans_speed);
  }
}

/*
  Translate and rotate polygon according to the amount of time that has passed
*/
void move_object_helper(state_t *state, double time) {

  body_t *star = (body_t *)list_get(state->polygon, 0);
  body_rotate(star, INIT_ROT_SPEED);
  body_set_centroid(star, vec_add(body_get_centroid(star),
                                  vec_multiply(1.0, body_get_velocity(star))));
  detect_wall_helper(state);
}

void create_star(state_t *state, double radius, double inner_radius_mult,
                 int vertices, vector_t centroid, vector_t trans_speed,
                 double rot_speed) {
  list_t *star = list_init(20, (free_func_t)free);

  for (int i = 0; i < vertices; i++) {
    list_add(star, malloc(sizeof(vector_t)));
  }

  *(vector_t *)list_get(star, 0) = (vector_t){0, radius}; // Reference point

  for (int i = 1; i < vertices; i++) {
    // Sets ith point to be Reference point rotated by (i * 2pi) / vertices
    *(vector_t *)list_get(star, i) =
        vec_rotate(*(vector_t *)list_get(star, 0), i * M_PI * 2 / vertices);
    // Sets every other point to be of inner radius
    if (i % 2 == 1) {
      *(vector_t *)list_get(star, i) =
          vec_multiply(inner_radius_mult, *(vector_t *)list_get(star, i));
    }
  }

  rgb_color_t yellow = {0, 0, 0};
  body_t *finished_star = body_init(star, 100, yellow);
  body_set_rotation(finished_star, rot_speed);
  body_set_velocity(finished_star, trans_speed);
  state->trans_speed = body_get_velocity(finished_star);
  body_set_centroid(finished_star, INIT_POSITION);
  list_add(state->polygon, finished_star);
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));

  state->polygon = list_init(1, (free_func_t)body_free);

  sdl_init(VEC_ZERO, (vector_t){WIN_WIDTH, WIN_HEIGHT});

  create_star(state, STAR_RADIUS, INNER_RADIUS_MULT, STAR_VERTICES * 2,
              INIT_POSITION, INIT_TRANS_SPEED, INIT_ROT_SPEED);

  sdl_draw_polygon(body_get_shape((body_t *)list_get(state->polygon, 0)),
                   (rgb_color_t){0, 0, 0});
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();

  move_object_helper(state, BIG_NUM * dt);

  sdl_clear();
  sdl_draw_polygon(body_get_shape((body_t *)list_get(state->polygon, 0)),
                   (rgb_color_t){0, 0, 0});
  sdl_show();
}

void emscripten_free(state_t *state) {
  list_free(state->polygon);
  free(state);
}