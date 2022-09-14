#include "body.h"
#include "list.h"
#include "sdl_wrapper.h"

const size_t WIN_WIDTH = 1000;
const size_t WIN_HEIGHT = 500;
const vector_t INIT_POSITION = {76, 400};
const vector_t INIT_TRANS_SPEED = {70, 0};
const double INIT_ROT_SPEED = 0.02;
const double STAR_RADIUS = 75;
const double INNER_RADIUS_MULT = 0.5;
const size_t STAR_VERTICES = 3;
const vector_t GRAVITY_VEC = {0, -20};
const double ELASTICITY_MULT = 0.92;
const double DEFAULT_MASS = 100;
const rgb_color_t BLACK = (rgb_color_t){0, 0, 0};