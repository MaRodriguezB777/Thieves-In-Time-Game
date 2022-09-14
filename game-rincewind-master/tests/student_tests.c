#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "forces.h"
#include "test_util.h"

list_t *make_shape() {
  list_t *shape = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){-1, -1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+1, -1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+1, +1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-1, +1};
  list_add(shape, v);
  return shape;
}

double gravity_potential(double G, body_t *body1, body_t *body2) {
  vector_t r = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
  return -G * body_get_mass(body1) * body_get_mass(body2) / sqrt(vec_dot(r, r));
}
double kinetic_energy(body_t *body) {
  vector_t v = body_get_velocity(body);
  return body_get_mass(body) * vec_dot(v, v) / 2;
}

double spring_potential_energy(body_t *mass1, body_t *mass2, double k) {
  vector_t vec1 = body_get_centroid(mass1);
  vector_t vec2 = body_get_centroid(mass2);
  double x = sqrt((vec1.x - vec2.x) * (vec1.x - vec2.x) +
                  (vec1.y - vec2.y) * (vec1.y - vec2.y));
  return 0.5 * k * pow(x, 2);
}

// Tests that a conservative force (gravity) conserves K + U
void test_spring_energy_conservation() {
  const double M1 = 10, M2 = 10;
  const double K = 10;
  const double DT = 1e-6;
  const int STEPS = 1000000;
  scene_t *scene = scene_init();
  body_t *mass1 = body_init(make_shape(), M1, (rgb_color_t){0, 0, 0});
  scene_add_body(scene, mass1);
  body_t *mass2 = body_init(make_shape(), M2, (rgb_color_t){0, 0, 0});
  body_set_centroid(mass2, (vector_t){10, 20});
  scene_add_body(scene, mass2);
  create_spring(scene, K, mass1, mass2);
  double initial_energy = spring_potential_energy(mass1, mass2, K);
  for (int i = 0; i < STEPS; i++) {
    double energy = spring_potential_energy(mass1, mass2, K) +
                    kinetic_energy(mass1) + kinetic_energy(mass2);
    assert(within(1e-4, energy / initial_energy, 1));
    scene_tick(scene, DT);
  }
  scene_free(scene);
}

void test_drag() {
  const double M = 4.5;
  const double DT = 1e-6;
  const double GAMMA = 100;
  const int STEPS = 1000000;
  scene_t *scene = scene_init();
  body_t *mass = body_init(make_shape(), M, (rgb_color_t){0, 0, 0});
  body_set_centroid(mass, (vector_t){0, 0});
  body_set_velocity(mass, (vector_t){10, 10});
  scene_add_body(scene, mass);
  create_drag(scene, GAMMA, mass);
  for (int i = 0; i < STEPS; i++) {
    scene_tick(scene, DT);
  }
  assert(vec_isclose(body_get_velocity(mass), VEC_ZERO));
  scene_free(scene);
}

// Using fomula to calculate
// gravity = G M*m / pow(r, 2) where G = 6.673 x 10 ^(-11), r = 6.4 x 10 ^ 6,  M
// = 6.018 x 10 ^24 for Earth Height = -1/2 * gravity * pow(t, 2) + v0*t + h0
// Velocity = -gravity * t + v0
void test_projectile_motion() {
  const double G = 6.67e-11;
  const double M = 6.018e24;
  const double r = 6.4e10;
  const double m = 10;
  const double VELOCITY_0 = 20;
  const double HEIGHT_0 = 5;
  const double DT = 1e-6;
  const int STEPS = 100000;

  scene_t *scene = scene_init();
  // object
  body_t *mass = body_init(make_shape(), m, (rgb_color_t){0, 0, 0});
  body_set_centroid(mass, (vector_t){0, HEIGHT_0});
  body_set_velocity(mass, (vector_t){0, VELOCITY_0});
  scene_add_body(scene, mass);
  // earth
  body_t *anchor = body_init(make_shape(), M, (rgb_color_t){0, 0, 0});
  body_set_centroid(anchor, (vector_t){0, -r});
  scene_add_body(scene, anchor);
  create_newtonian_gravity(scene, G, mass, anchor);
  double gavity = G * M * m / (r * r);
  for (int i = 0; i < STEPS; i++) {

    double t = i * DT;
    double height = -0.5 * gavity * pow(t, 2) + VELOCITY_0 * t + HEIGHT_0;
    double velocity = -gavity * t + VELOCITY_0;
    assert(within(1e-4, height, body_get_centroid(mass).y));
    assert(within(1e-4, velocity, body_get_velocity(mass).y));
    scene_tick(scene, DT);
  }
  scene_free(scene);
}

int main(int argc, char *argv[]) {
  // Run all tests if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }

  DO_TEST(test_spring_energy_conservation)
  DO_TEST(test_drag)
  DO_TEST(test_projectile_motion)

  puts("forces_test PASS");
}
