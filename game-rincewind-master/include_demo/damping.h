#include "forces.h"
#include "sdl_wrapper.h"

const size_t WIN_WIDTH = 1000;
const size_t WIN_HEIGHT = 500;
const double TIME_MULT = 50;

const double BALL_RADIUS = 10;
const size_t BALL_VERTICES = 32;
const double BALL_MASS = 50;

const size_t CENTER_VERTICES = 3;

const double RATIO_BALLS = 1.0 / 3;
const double SPRING_K_LEFT = 0.5;
const double GAMMA_LEFT = 0.3;
const double SHIFT_MULT_L = 3.0 / 4;
const double AMP_MULT_L = 1.0 / 4;
const double COS_MULT_L = 3;
const double SPRING_K_RIGHT = 0.01;
const double GAMMA_RIGHT = 0.8;
const double SHIFT_MULT_R = 9.0 / 16;
const double AMP_MULT_R = 1.0 / 16;
const double COS_MULT_R = 3;

// Color
const rgb_color_t COLOR_RED = {255, 0, 0};
const rgb_color_t BLACK = {0, 0, 0};
const double COLOR_FREQ = .3;
const double RED_SHIFT = 0;
const double GREEN_SHIFT = 2;
const double BLUE_SHIFT = 4;