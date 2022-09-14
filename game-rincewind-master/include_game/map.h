#ifndef __MAP_H__
#define __MAP_H__

#include "game_const.h"
#include "player.h"

void generate_menu(scene_t *scene);

void generate_map1(scene_t *scene);

void generate_map2(scene_t *scene);

void create_map(scene_t *scene, game_state_t game_state);

void check_bounds(body_t *body);

#endif // #ifndef __MAP_H__