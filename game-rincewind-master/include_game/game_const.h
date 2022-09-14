#ifndef __GAME_CONST__
#define __GAME_CONST__

#include "color.h"
#include "vector.h"

// Map
extern const vector_t MAX1;
extern const vector_t MAX2;
extern const vector_t MAX_MENU;

// Player
extern const double PLAYER_MASS;
extern const rgb_color_t PLAYER_1_COLOR;
extern const rgb_color_t PLAYER_2_COLOR;

// Powerups
extern const int MAX_POWERUPS;

// Gravity
extern const double G; // N m^2 / kg^2

#endif // #ifndef __GAME_CONST_H__