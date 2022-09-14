#include "damping.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct state {
  scene_t *scene;
  list_t *spring_centers;
} state_t;

/**
 * @brief returns colors of a rainbow in order given an index
 *
 * @param num_of_balls
 * @param index
 * @return color with 0 <= red, green, blue <= 1
 */
rgb_color_t rainbow_color(size_t num_of_balls, size_t index) {
  float red = sin(COLOR_FREQ * index + RED_SHIFT) + 1;
  float green = sin(COLOR_FREQ * index + GREEN_SHIFT) + 1;
  float blue = sin(COLOR_FREQ * index + BLUE_SHIFT) + 1;
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
  return (rgb_color_t){red, green, blue};
}

list_t *make_ball_shape(double radius) {
  list_t *shape = list_init(BALL_VERTICES, (free_func_t)free);
  vector_t *ref = malloc(sizeof(vector_t));
  *ref = (vector_t){radius, 0};
  list_add(shape, ref);

  for (size_t i = 1; i < BALL_VERTICES; i++) {
    vector_t *vertex = malloc(sizeof(vector_t));
    *vertex = vec_rotate(*ref, 2 * M_PI * i / BALL_VERTICES);
    list_add(shape, vertex);
  }

  return shape;
}

void make_balls(scene_t *scene, double radius, size_t num_balls) {
  vector_t ref = {-1 * radius, 0};
  for (size_t i = 0; i < num_balls; i++) {
    body_t *body = body_init(make_ball_shape(radius), BALL_MASS,
                             rainbow_color(num_balls, i));

    ref.x += 2 * radius;
    if (i <= num_balls * RATIO_BALLS) {
      ref.y =
          (WIN_HEIGHT * SHIFT_MULT_L) +
          (WIN_HEIGHT * AMP_MULT_L) * cos(i * COS_MULT_L * M_PI / num_balls) -
          radius;
    } else if (i > num_balls * (1 - RATIO_BALLS)) {
      ref.y =
          (WIN_HEIGHT * SHIFT_MULT_R) -
          (WIN_HEIGHT * AMP_MULT_R) * cos(i * COS_MULT_R * M_PI / num_balls) -
          radius;
    } else {
      ref.y = WIN_HEIGHT / 2 - radius;
    }

    body_set_centroid(body, ref);
    scene_add_body(scene, body);
  }
}

void add_forces_left(scene_t *scene, list_t *centers, double k, double GAMMA) {
  for (size_t i = 0; i < scene_bodies(scene) * RATIO_BALLS; i++) {
    body_t *body = scene_get_body(scene, i);
    list_t *center_shape = make_ball_shape(CENTER_VERTICES);

    body_t *center = body_init(center_shape, INFINITY, BLACK);
    body_set_centroid(center, (vector_t){body_get_centroid(body).x,
                                         WIN_HEIGHT / 2 - BALL_RADIUS});
    list_add(centers, center);
    create_spring(scene, k, body, center);
    create_drag(scene, GAMMA, body);
  }
}

void add_forces_right(scene_t *scene, list_t *centers, double k, double gamma,
                      double percent_balls_to_damp) {
  size_t num_balls_damped =
      (size_t)(scene_bodies(scene) * percent_balls_to_damp);
  size_t ind = scene_bodies(scene) - num_balls_damped;

  for (size_t i = ind; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    list_t *center_shape = make_ball_shape(CENTER_VERTICES);

    body_t *center = body_init(center_shape, INFINITY, BLACK);
    body_set_centroid(center, (vector_t){body_get_centroid(body).x,
                                         WIN_HEIGHT / 2 - BALL_RADIUS});
    list_add(centers, center);
    create_spring(scene, k, body, center);
    create_drag(scene, gamma, body);
  }
}

/**
 * @brief Initialise and allocate memory for state.
 * Initialise scene, balls, forces, and
 * time.
 * @return the current state
 */
state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));

  sdl_init(VEC_ZERO, (vector_t){WIN_WIDTH, WIN_HEIGHT});
  scene_t *scene = scene_init();
  state->scene = scene;
  size_t tot_balls = (size_t)(WIN_WIDTH / (2 * BALL_RADIUS));
  state->spring_centers = list_init(tot_balls, (free_func_t)body_free);
  make_balls(state->scene, BALL_RADIUS, tot_balls);
  add_forces_left(state->scene, state->spring_centers, SPRING_K_LEFT,
                  GAMMA_LEFT);
  add_forces_right(state->scene, state->spring_centers, SPRING_K_RIGHT,
                   GAMMA_RIGHT, RATIO_BALLS);

  sdl_render_scene(scene);

  return state;
}

/**
 * @brief Increments time and draws each frame
 * @param state state_t describing current frame
 */
void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  scene_tick(state->scene, TIME_MULT * dt);
  sdl_render_scene(state->scene);
}

/**
 * @brief Frees all bodies from scene, and all bodies from spring_centers
 * @param state state_t describing the current frame
 */
void emscripten_free(state_t *state) {
  scene_free(state->scene);
  list_free(state->spring_centers);
  free(state);
}