#ifndef __GAME_H__
#define __GAME_H__

#include <stddef.h>

typedef enum game_weapon_type {
  PISTOL,
  RICOCHET,
  SHOTGUN,
  NO_WEAPON
} game_weapon_type_t;

typedef enum game_state {
  INTRO_MENU,
  MAIN_MENU,
  LORE,
  MODE_SELECT,
  MAP_SELECT,
  CREDITS,
  INSTRUCTIONS,
  MAP1,
  MAP2,
  MAP3,
  GAME_WIN_P1,
  GAME_WIN_P2
} game_state_t;

typedef enum side { LEFT, RIGHT, UP, DOWN, NO_SIDE } side_t;

typedef enum sound {
  PISTOL_S,
  SHOTGUN_S,
  RICOCHET_S,
  JUMP,
  CLICK,
  HIT,
  MENU_MUS,
  MAP1_MUS,
  MAP2_MUS,
  END_MUS,
  MAP3_MUS
} sound_t;

typedef enum body_type {
  PLAYER1,
  PLAYER2,
  WALL,
  GROUND,
  BULLET,
  GRAVITY,
  BACKGROUND,
  POWERUP_RICOCHET,
  POWERUP_SHOTGUN,
  CLOCK_SMALL_ARM,
  CLOCK_BIG_ARM,
  CLOCK,
  P1_LIFE,
  P2_LIFE
} body_type_t;

typedef struct body_info {
  body_type_t type;
  side_t side;
  game_weapon_type_t weapon_type;
  double time_since_last_shot;
  size_t shots_left;
} body_info_t;

#endif // #ifndef __GAME_H__