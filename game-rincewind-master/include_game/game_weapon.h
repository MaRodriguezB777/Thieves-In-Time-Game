#ifndef __GAME_WEAPON_H__
#define __GAME_WEAPON_H__

#include "forces.h"
#include "game.h"

void game_weapon_upgrade(body_t *player, game_weapon_type_t upgrade);

bool spawn_powerup(scene_t *scene, double time_since_last_drop,
                   double powerups_on_screen, game_state_t map);

bool game_weapon_shoot(scene_t *scene, body_t *player);

typedef struct collision_aux_radial collision_aux_radial_t;

/**
 * @brief Adds a force creator between a player and a powerup such that when
 * they collide, the player will be given an upgraded weapon.
 *
 * @throw Assertion error if type of player is not 'PLAYER' and type of
 * powerup is not 'POWERUP'
 * @param scene the scene containing the bodies
 * @param player the player body
 * @param powerup the powerup body
 */
void create_powerup_pickup_collision(scene_t *scene, body_t *player,
                                     body_t *powerup);

/**
 * @brief Adds a force creator between two bodies that can destroy the bodies
 * if they get too far away from each other
 *
 * @param scene the scene where the bodies are contained
 * @param body1 a body object
 * @param body2 a body object
 * @param body1_destroyable whether body1 will be destroyed during the collision
 * @param body2_destroyable whether body2 will be destroyed during the collision
 * @param radius how far away the bodies are allowed to be apart
 */
void create_radial_destructive_collision(scene_t *scene, body_t *body1,
                                         body_t *body2, bool body1_destroyable,
                                         bool body2_destroyable, double radius);

/**
 * @brief Destroys bodies if they are too far apart from one another
 *
 * @param void_aux contains two bodies, radius and which bodies should be
 * destroyed
 */
void calc_radial_destructive_collision(void *void_aux);

/**
 * @brief Performs destructive collision on powerup and give player
 * a new weapon.
 *
 * @param player the player body
 * @param powerup the powerup body
 * @param axis the axis of collision from the player to the powerup
 * @param void_aux empty
 */
void calc_pickup_collision(body_t *player, body_t *powerup, vector_t axis,
                           void *void_aux);

#endif // #ifndef __GAME_WEAPON_H__
