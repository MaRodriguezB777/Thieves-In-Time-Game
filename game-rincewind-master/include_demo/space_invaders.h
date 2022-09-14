#include "collision.h"
#include "forces.h"
#include "sdl_wrapper.h"

const size_t WIN_WIDTH = 1000;
const size_t WIN_HEIGHT = 500;
const size_t TIME_MULT = 50;
const double LOSE_HEIGHT = 50;

// Bullet Information
const double B_VERTICES = 4;
const double B_HEIGHT = 10;
const double B_WIDTH = 3;
const vector_t P_B_VELOCITY = {0, 15};
const vector_t INVDR_B_VELOCITY = {0, -10};
const rgb_color_t P_B_COLOR = {0, 1, 0};
const rgb_color_t INVDR_B_COLOR = {1, 0, 0};
const double B_MASS = 10;

// Player Information
const double P_HEIGHT = 10.0;
const double P_WIDTH = 20;
const rgb_color_t P_COLOR = {1, 0, 1};
const double P_SPEED = 10;
const double P_VERTICES = 32;
const double P_MASS = 100;

// Invader Information
const double INVDR_RADIUS = 30;
const size_t INVDR_VERTICES = 32;
const double INVDR_ARC_MULT = 1.0 / 3;
const double INVDR_INIT_SPAWN_HEIGHT = WIN_HEIGHT - 30;
const double INVDR_BUFFER = 3;
const rgb_color_t INVDR_COLOR = {0.7, 0.7, 0.7};
const size_t INVDR_NUM_OF_ROWS = 3;
const double INVDR_SPEED = 3;
const double INVDR_MASS = 100;