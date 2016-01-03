#include "actor.h"

actor create_actor_from_resource(char* resource_location, char* resource_name);
void update_actor(actor a, float delta);
void update_actors(float delta);
void draw_actor(actor a);
void draw_actors();
void destroy_actor(actor a);
void destroy_actors();
uint8_t can_move(actor a);
void actor_set_anim_index(actor a, int8_t index);
void actor_callback(actor a, uint8_t type, void* data);
void actor_operate(actor a);
uint8_t actor_orients(actor a);
void move_actor_maps(int16_t x, int16_t y);
