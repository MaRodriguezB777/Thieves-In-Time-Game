#ifndef __FORCE_CREATOR_H__
#define __FORCE_CREATOR_H__

#include "body.h"
#include "collision.h"
#include "vector.h"

/**
 * A function called when a collision occurs.
 * @param body1 the first body passed to create_collision()
 * @param body2 the second body passed to create_collision()
 * @param axis a unit vector pointing from body1 towards body2
 *   that defines the direction the two bodies are colliding in
 * @param aux the auxiliary value passed to create_collision()
 */
typedef void (*collision_handler_t)(body_t *body1, body_t *body2, vector_t axis,
                                    void *aux);

typedef struct force_aux_2bodies force_aux_2bodies_t;

typedef struct force_aux_1body force_aux_1body_t;

typedef struct force_aux_collision_bodies force_aux_collision_bodies_t;

typedef struct force_aux_collision force_aux_collision_t;

typedef struct collision_aux_destructive collision_aux_destructive_t;

typedef struct collision_aux_physics collision_aux_physics_t;

force_aux_1body_t *force_aux_1body_init(double constant, body_t *body);

force_aux_2bodies_t *force_aux_2bodies_init(double constant, body_t *body1,
                                            body_t *body2);

force_aux_collision_bodies_t *force_aux_collision_bodies_init(body_t *body1,
                                                              body_t *body_2);

force_aux_collision_t *force_aux_collision_init(body_t *body1, body_t *body2,
                                                collision_handler_t handler,
                                                void *aux, free_func_t freer);

collision_aux_destructive_t *
collision_aux_destructive_init(bool body1_is_destructive,
                               bool body2_is_destructive,
                               size_t coll_before_destruct);

collision_aux_physics_t *collision_aux_physics_init(double elasticity);

void calc_gravity(void *aux);

void calc_spring(void *aux);

void calc_drag(void *aux);

void calc_normal_force(void *void_aux);

void calc_collision(void *aux);

void calc_destructive_collision(body_t *body1, body_t *body2, vector_t axis,
                                void *aux);

void calc_physics_collision(body_t *body1, body_t *body2, vector_t axis,
                            void *aux);

void standard_free_aux(void *aux);

void free_aux_collision(void *aux);

#endif // #ifndef __FORCE_CREATOR_H__