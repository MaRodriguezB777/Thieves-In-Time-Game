#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "game_weapon.h"

body_info_t *info_init(body_type_t type, side_t side,
                       game_weapon_type_t weapon);

body_info_t *get_info(body_t *body);

body_t *fetch_object(scene_t *scene, body_type_t body_type);

sprite_t *fetch_sprite(scene_t *scene, body_type_t body_type);

body_t *add_player(scene_t *scene, body_type_t type, vector_t spawn);

body_t *get_player_feet(body_t *player);

#endif // #ifndef __PLAYER_H__