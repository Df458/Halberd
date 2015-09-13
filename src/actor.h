#ifndef ACTOR_H
#define ACTOR_H
#include <inttypes.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "render_util.h"

#define FLAG_SOLID            0b10000000
#define FLAG_VISIBLE          0b01000000
#define FLAG_LOCK_TO_GRID     0b00100000
#define FLAG_CAN_ORIENT       0b00010000
#define FLAG_BLOCK_WITH_SOLID 0b00001000

#define ORIENT_NORTH 0
#define ORIENT_EAST  1
#define ORIENT_SOUTH 2
#define ORIENT_WEST  3
#define ORIENT_NORTHEAST  4
#define ORIENT_SOUTHEAST  5
#define ORIENT_SOUTHWEST  6
#define ORIENT_NORTHWEST  7

enum callback_types
{ 
    CALLBACK_CREATED,
    CALLBACK_DESTROYED,
    CALLBACK_COLLISION,
    CALLBACK_TICK,
    CALLBACK_OPERATED,
    CALLBACK_COUNT
};

static const char* const CALLBACK_NAMES[] =
{
    "created",
    "destroyed",
    "collision",
    "tick",
    "operated",
    "count",
};

struct actordata
{
    uint32_t position_x;
    uint32_t position_y;
    int16_t grid_x;
    int16_t grid_y;
    int16_t super_grid_x;
    int16_t super_grid_y;
    uint16_t speed;
    uint8_t moving;
    uint8_t orientation;
    uint8_t flags;
    int8_t animation_index;
    uint8_t animation_playing;
    float animation_timer;

    spriteset* sprites;
};

typedef struct actor
{
    struct actordata data;
    lua_State* callbacks[CALLBACK_COUNT];
} actor;

actor* create_actor(const char* path);
void update_actor(actor* a, float delta);
void update_actors(float delta);
void draw_actor(actor* a);
void draw_actors();
void destroy_actor(actor* a);
void destroy_actors();
uint8_t can_move(actor* a);
void actor_set_anim_index(actor* a, int8_t index);
void actor_callback(actor* a, uint8_t type, void* data);
void actor_operate(actor* a);
uint8_t actor_orients(actor* a);
void move_actor_maps(int16_t x, int16_t y);

#endif
