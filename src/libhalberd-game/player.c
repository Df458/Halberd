#include "player.h"
#include "io.h"
#include "map.h"
#include "texture_util.h"

#include <stdlib.h>
#include <dfgame-game.h>

actor player_actor;

void init_player(uint32_t x, uint32_t y)
{
    player_actor = malloc(sizeof(struct Actor));
    player_actor->data.flags = FLAG_SOLID | FLAG_VISIBLE | FLAG_LOCK_TO_GRID | FLAG_CAN_ORIENT | FLAG_BLOCK_WITH_SOLID;
    player_actor->data.orientation = ORIENT_SOUTH;
    player_actor->data.grid_x = x;
    player_actor->data.grid_y = y;
    player_actor->data.super_grid_x = 1;
    player_actor->data.super_grid_y = 1;
    player_actor->data.position_x = x * TILE_WIDTH;
    player_actor->data.position_y = y * TILE_HEIGHT;
    player_actor->data.speed = 3;
    player_actor->data.animation_timer = 0;
    player_actor->data.animation_index = 0;
    // TODO: Figure out how to properly get player info
    player_actor->data.sprites = load_resource_to_sprite(NULL, "test.spr");
    if(player_actor->data.sprites != 0) {
        player_actor->data.animation_index = 0;
        player_actor->data.animation_playing = player_actor->data.sprites->animations[0].play;
    }
    for(uint8_t i = 0; i < CALLBACK_COUNT; ++i)
        player_actor->callbacks[i] = 0;
}

void update_player(float delta)
{
    int direction;
    if(player_actor->data.moving == 0 && get_action_input()) {
        actor_operate(player_actor);
    } else if(player_actor->data.moving == 0 && (direction = get_directional_input())) {
        uint8_t o = player_actor->data.orientation;
        switch(direction) {
            case 1: player_actor->data.orientation = ORIENT_WEST; break;
            case 2: player_actor->data.orientation = ORIENT_EAST; break;
            case 3: player_actor->data.orientation = ORIENT_NORTH; break;
            case 4: player_actor->data.orientation = ORIENT_SOUTH; break;
        }
        if(player_actor->data.orientation == o)
            player_actor->data.moving = 1;
    }
    update_actor(player_actor, delta);
    if(player_actor->data.super_grid_x != 1 || player_actor->data.super_grid_y != 1) {
        move_maps(1 - player_actor->data.super_grid_x, 1 - player_actor->data.super_grid_y);
        move_actor_maps(1 - player_actor->data.super_grid_x, 1 - player_actor->data.super_grid_y);
        player_actor->data.super_grid_x = 1;
        player_actor->data.super_grid_y = 1;
    }
    uint16_t view_width = 0;
    uint16_t view_height = 0;
    get_viewport_dimensions(&view_width, &view_height);
    update_view((player_actor->data.position_x + (TILE_WIDTH * TILEMAP_DIMS) - (view_width / 2)), (player_actor->data.position_y + TILE_HEIGHT * TILEMAP_DIMS - (view_height / 2)), 1);
}

void draw_player()
{
    draw_actor(player_actor);
}

void destroy_player_actor()
{
}
