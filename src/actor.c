#include "actor.h"
#include "io.h"
#include "map.h"
#include "render_util.h"
#include <stdlib.h>
#include <stdio.h>

actor** actor_list = 0;
uint32_t actor_count = 0;
uint32_t available_count = 0;

actor* create_actor(const char* path)
{
    if(available_count <= actor_count) {
        available_count += 5;
        actor_list = realloc(actor_list, available_count * sizeof(actor*));
    }
    actor_count++;
    actor_list[actor_count - 1] = load_actor(path);
    actor_callback(actor_list[actor_count - 1], CALLBACK_CREATED, (void*)0);
    return actor_list[actor_count - 1];
}

void update_actor(actor* a, float delta)
{   
    if(!a)
        return;

    actor_callback(a, CALLBACK_TICK, (void*)&delta);
    if(a->data.moving && a->data.position_x == a->data.grid_x * TILE_WIDTH && a->data.position_y == a->data.grid_y * TILE_HEIGHT && !can_move(a)) {
        a->data.moving = 0;
    }
    if(a->data.moving) {
        if(a->data.animation_index == 0) {
            actor_set_anim_index(a, index_by_handle(a->data.sprites, "walk"));
        }
        switch(a->data.orientation) {
            case 0:
                a->data.position_y -= a->data.speed;
                break;
            case 1:
                a->data.position_x += a->data.speed;
                break;
            case 2:
                a->data.position_y += a->data.speed;
                break;
            case 3:
                a->data.position_x -= a->data.speed;
                break;
            case 4:
                a->data.position_x += a->data.speed;
                a->data.position_y -= a->data.speed;
                break;
            case 5:
                a->data.position_x += a->data.speed;
                a->data.position_y += a->data.speed;
                break;
            case 6:
                a->data.position_x -= a->data.speed;
                a->data.position_y += a->data.speed;
                break;
            case 7:
                a->data.position_x -= a->data.speed;
                a->data.position_y -= a->data.speed;
                break;
        }
        if(abs(a->data.position_x - a->data.grid_x * TILE_WIDTH) >= TILE_WIDTH || abs(a->data.position_y - a->data.grid_y * TILE_HEIGHT) >= TILE_HEIGHT) {
            a->data.moving = 0;
            a->data.grid_x += (a->data.position_x - a->data.grid_x * TILE_WIDTH + TILE_WIDTH / 2) / TILE_WIDTH;
            a->data.grid_y += (a->data.position_y - a->data.grid_y * TILE_HEIGHT + TILE_HEIGHT / 2) / TILE_HEIGHT;

            while(a->data.grid_x >= TILEMAP_DIMS) {
                a->data.grid_x -= TILEMAP_DIMS;
                a->data.position_x = a->data.grid_x * TILE_WIDTH;
                a->data.super_grid_x++;
            }
            while(a->data.grid_y >= TILEMAP_DIMS) {
                a->data.grid_y -= TILEMAP_DIMS;
                a->data.position_y = a->data.grid_y * TILE_HEIGHT;
                a->data.super_grid_y++;
            }
            while(a->data.grid_x < 0) {
                a->data.grid_x += TILEMAP_DIMS;
                a->data.position_x = a->data.grid_x * TILE_WIDTH;
                a->data.super_grid_x--;
            }
            while(a->data.grid_y < 0) {
                a->data.grid_y += TILEMAP_DIMS;
                a->data.position_y = a->data.grid_y * TILE_HEIGHT;
                a->data.super_grid_y--;
            }
            a->data.position_x = a->data.grid_x * TILE_WIDTH;
            a->data.position_y = a->data.grid_y * TILE_HEIGHT;
        }
    } else {
        if(a->data.animation_index != 0) {
            a->data.animation_index = 0;
            a->data.animation_timer = 0;
            actor_set_anim_index(a,  0);
        }
    }

    // Update animation
    if(a->data.animation_index == -1)
        actor_set_anim_index(a,  0);
    if(a->data.animation_playing)
        a->data.animation_timer += delta * 10;
    if((int)a->data.animation_timer / a->data.sprites->animations[a->data.animation_index].delay >= a->data.sprites->animations[a->data.animation_index].length) {
        if(a->data.sprites->animations[a->data.animation_index].loop){
            a->data.animation_timer = 0;
        }else{
            a->data.animation_timer = (a->data.sprites->animations[a->data.animation_index].length - 1) * a->data.sprites->animations[a->data.animation_index].delay;
            a->data.animation_playing = 0;
        }
    }
}

void actor_set_anim_index(actor* a, int8_t index)
{
    if(!a || a->data.animation_index == index)
        return;
    if(index < 0 || index > a->data.sprites->animation_count)
        index = 0;
    // TODO: Set the animation
    a->data.animation_index = index;
    a->data.animation_timer = 0;
    a->data.animation_playing = a->data.sprites->animations[index].play;
}

void update_actors(float delta)
{
    for(int i = 0; i < actor_count; ++i)
        update_actor(actor_list[i], delta);
}

void draw_actor(actor* a)
{
    if(!(a->data.flags & FLAG_VISIBLE))
        return;
    struct color col;
    col.r = 1;
    col.g = 1;
    col.b = 1;
    col.a = 1;

    draw_spriteset(a->data.sprites, a->data.animation_index, a->data.animation_timer / a->data.sprites->animations[a->data.animation_index].delay, a->data.orientation, actor_orients(a), a->data.position_x + a->data.super_grid_x * TILE_WIDTH * TILEMAP_DIMS, a->data.position_y + a->data.super_grid_y * TILE_HEIGHT * TILEMAP_DIMS, 0, 1, 1, col);
}

void draw_actors()
{
    for(int i = 0; i < actor_count; ++i) {
        draw_actor(actor_list[i]);
    }
}

void destroy_actor(actor* a)
{
    for(uint16_t i = 0; i < CALLBACK_COUNT; ++i)
        if(a->callbacks[i])
            lua_close(a->callbacks[i]);
    if(a->data.sprites)
        destroy_spriteset(a->data.sprites);
    free(a);
}

void destroy_actors()
{
    for(uint32_t i = 0; i < actor_count; ++i)
        destroy_actor(actor_list[i]);
    free(actor_list);
}

uint8_t can_move(actor* a)
{
    if(!(a->data.flags & FLAG_BLOCK_WITH_SOLID))
        return 1;

    int16_t next_x = a->data.grid_x;
    int16_t next_y = a->data.grid_y;
    uint16_t next_super_x = a->data.super_grid_x;
    uint16_t next_super_y = a->data.super_grid_y;

    switch(a->data.orientation) {
        case 0:
            next_y--;
            break;
        case 1:
            next_x++;
            break;
        case 2:
            next_y++;
            break;
        case 3:
            next_x--;
            break;
        case 4:
            next_x++;
            next_y--;
            break;
        case 5:
            next_x++;
            next_y++;
            break;
        case 6:
            next_x--;
            next_y++;
            break;
        case 7:
            next_x--;
            next_y--;
            break;
    }
    get_true_tile_position(&next_super_x, &next_super_y, &next_x, &next_y);
    
    // FIXME: This will likely break if two actors try to move to the same spot
    for(int i = 0; i < actor_count; ++i) {
        if(actor_list[i]->data.flags & FLAG_SOLID && actor_list[i]->data.grid_x == next_x && actor_list[i]->data.grid_y == next_y && actor_list[i]->data.super_grid_x == next_super_x && actor_list[i]->data.super_grid_y == next_super_y)
            return 0;
    }

    return get_solid(get_tile(next_super_x, next_super_y, next_x, next_y, 0));
}

void actor_callback(actor* a, uint8_t type, void* data)
{
    // TODO: Use extra data passed in
    if(a->callbacks[type]) {
        lua_getglobal(a->callbacks[type], CALLBACK_NAMES[type]);
        int errs = lua_pcall(a->callbacks[type], 0, LUA_MULTRET, 0);
        while(errs) {
            fprintf(stderr, "lua error: %s\n", lua_tostring(a->callbacks[type], -1));
            lua_pop(a->callbacks[type], 1);
            --errs;
        }
    }
}

void actor_operate(actor* a)
{
    uint16_t next_x = a->data.grid_x;
    uint16_t next_y = a->data.grid_y;

    switch(a->data.orientation) {
        case 0:
            next_y--;
            break;
        case 1:
            next_x++;
            break;
        case 2:
            next_y++;
            break;
        case 3:
            next_x--;
            break;
        case 4:
            next_x++;
            next_y--;
            break;
        case 5:
            next_x++;
            next_y++;
            break;
        case 6:
            next_x--;
            next_y++;
            break;
        case 7:
            next_x--;
            next_y--;
            break;
    }

    for(int i = 0; i < actor_count; ++i) {
        if(actor_list[i]->data.grid_x == next_x && actor_list[i]->data.grid_y == next_y)
            actor_callback(actor_list[i], CALLBACK_OPERATED, (void*)a);
    }
}

uint8_t actor_orients(actor* a)
{
    if(!(a->data.flags & FLAG_CAN_ORIENT))
        return 1;
    if(a->data.flags & FLAG_LOCK_TO_GRID)
        return 4;
    return 8;
}

void move_actor_maps(int16_t x, int16_t y)
{
    for(int i = 0; i < actor_count; ++i) {
        actor_list[i]->data.super_grid_x += x;
        actor_list[i]->data.super_grid_y += y;
    }
}
