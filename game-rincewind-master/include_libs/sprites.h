#ifndef __SPRITES_H__
#define __SPRITES_H__

#include "body.h"
#include "list.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct sprite sprite_t;

// void list_of_sprites_free(sprite_t *sprite);

sprite_t *sprite_init(body_t *body);

void sprite_update(sprite_t *sprite);

body_t *sprite_get_body(sprite_t *sprite);

SDL_Rect *sprite_get_destR(sprite_t *sprite);

void sprite_set_destR(sprite_t *sprite, SDL_Rect destR);

bool sprite_is_removed(sprite_t *sprite);

void sprite_free(sprite_t *sprite);

vector_t get_window_center(void);

vector_t get_window_position(vector_t scene_pos, vector_t window_center);

double get_scene_scale(vector_t window_center);

// Texture
void sprite_add_tex(sprite_t *sprite, SDL_Texture *tex);

SDL_Texture *sprite_get_tex(sprite_t *sprite, size_t index);

size_t sprite_textures(sprite_t *sprite);

void sprite_set_tex(sprite_t *sprite, size_t index);

size_t sprite_get_curr_ind(sprite_t *sprite);

#endif // #ifndef __SPRITES_H__