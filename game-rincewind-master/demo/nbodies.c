#include "nbodies.h"
#include "forces.h"
#include <assert.h>
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

/**
 * @brief Initialise and allocate memory for state.
 *
 * @return the current state
 */
body_t *create_star(size_t vertices) {
  list_t *star = list_init(vertices, (free_func_t)free);

  for (size_t i = 0; i < vertices; i++) {
    list_add(star, malloc(sizeof(vector_t)));
  }

  double radius = rand() % RADIUS_VARIATION + MIN_RADIUS;
  *(vector_t *)list_get(star, 0) = (vector_t){0, radius}; // Reference point

  for (size_t i = 1; i < vertices; i++) {
    // Sets ith point to be Reference point rotated by (i * 2pi) / vertices
    *(vector_t *)list_get(star, i) =
        vec_rotate(*(vector_t *)list_get(star, 0), i * M_PI * 2 / vertices);
    // Sets every other point to be of inner radius
    if (i % 2 == 1) {
      *(vector_t *)list_get(star, i) =
          vec_multiply(INNER_RADIUS_MULT, *(vector_t *)list_get(star, i));
    }
  }

  // color star
  rgb_color_t rand_color = {(rand() % 255) / 255.0, (rand() % 255) / 255.0,
                            (rand() % 255) / 255.0};
  body_t *finished_star = body_init(
      star, ((rand() % (MAX_MASS - MIN_MASS)) + MIN_MASS), rand_color);
  body_set_rotation(finished_star, INITIAL_ROT_SPEED);
  body_set_velocity(finished_star, VEC_ZERO);

  // place star in random location
  vector_t init_position = {(double)(rand() % WIN_WIDTH),
                            (double)(rand() % WIN_HEIGHT)};
  body_set_centroid(finished_star, init_position);

  return finished_star;
}

/**
 * @brief Initialise and allocate memory for state.
 * Initialise scene, pacman, first set of pellets, and
 * time.
 * @return the current state
 */
scene_t *generate_scene() {
  scene_t *scene = scene_init();
  for (size_t i = 0; i < BODY_COUNT; i++) {
    scene_add_body(scene, create_star(VERTICES));
  }

  return scene;
}

/**
 * @brief Initialise and allocate memory for state.
 * Initialise scene, pacman, first set of pellets, and
 * time.
 * @return the current state
 */
state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  sdl_init(VEC_ZERO, (vector_t){WIN_WIDTH, WIN_HEIGHT});
  state->scene = generate_scene();

  for (size_t i = 0; i < scene_bodies(state->scene) - 1; i++) {
    for (size_t j = i + 1; j < scene_bodies(state->scene); j++) {
      body_t *body1 = scene_get_body(state->scene, i);
      body_t *body2 = scene_get_body(state->scene, j);
      create_newtonian_gravity(state->scene, GRAVITY_NBODIES, body1, body2);
    }
  }

  sdl_render_scene(state->scene);

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
 * @brief Frees all polygons from polygon list, then frees polygon list itself
 * finally frees the entire state
 * @param state state_t describing the current frame
 */
void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}
