#include "sdl_wrapper.h"
#include "state.h"

const size_t WIN_WIDTH = 1000;
const size_t WIN_HEIGHT = 500;
const vector_t INIT_POSITION = {500, 250};
const double COLLISION_RADIUS = 30.0;
const size_t PELLET_START = 15;
const double ACCELERATION = 1;
const double RADIUS = 50;
const int DEFAULT_SPEED = 5;
const int DEFAULT_MASS = 100;
const vector_t DEFAULT_LEFT_SPEED = {-DEFAULT_SPEED, 0};
const vector_t DEFAULT_UP_SPEED = {0, DEFAULT_SPEED};
const vector_t DEFAULT_RIGHT_SPEED = {DEFAULT_SPEED, 0};
const vector_t DEFAULT_DOWN_SPEED = {0, -DEFAULT_SPEED};

const int SPHERE_VERTICES = 32;
const int MOUTH_VERTICES = 6;
const rgb_color_t PACMAN_COLOR = {1, 1, 0};
const rgb_color_t PACMAN_2_COLOR = {1, 0, 1};

// Pellets
const size_t PELL_VERTICES = 8;
const vector_t PELL_INIT_VECTOR = {10, 0};
const double PELL_MASS = 1.0;
const rgb_color_t PELL_COLOR = {1, 1, 0};
const int PELL_SPAWN_RATE = 20;