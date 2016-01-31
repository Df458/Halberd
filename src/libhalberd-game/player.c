#include "player.h"
#include "controls.h"
#include "io.h"
#include "map.h"
#include "texture_util.h"

#include <stdlib.h>

actor player;

void init_player(uint32_t x, uint32_t y)
{
    player = malloc(sizeof(struct Actor));
    player->data.flags = FLAG_SOLID | FLAG_VISIBLE | FLAG_LOCK_TO_GRID | FLAG_CAN_ORIENT | FLAG_BLOCK_WITH_SOLID;
    player->data.orientation = ORIENT_SOUTH;
    player->data.grid_x = x;
    player->data.grid_y = y;
    player->data.super_grid_x = 1;
    player->data.super_grid_y = 1;
    player->data.position_x = x * TILE_WIDTH;
    player->data.position_y = y * TILE_HEIGHT;
    player->data.speed = 3;
    player->data.animation_timer = 0;
    player->data.animation_index = 0;
    // TODO: Figure out how to properly get player info
    player->data.sprites = load_resource_to_sprite(NULL, "test.spr");
    if(player->data.sprites != 0) {
        player->data.animation_index = 0;
        player->data.animation_playing = player->data.sprites->animations[0].play;
    }
    for(uint8_t i = 0; i < CALLBACK_COUNT; ++i)
        player->callbacks[i] = 0;
}

void update_player(float delta)
{
    int direction;
    if(player->data.moving == 0 && get_action_input()) {
        actor_operate(player);
    } else if(player->data.moving == 0 && (direction = get_directional_input())) {
        uint8_t o = player->data.orientation;
        switch(direction) {
            case 1: player->data.orientation = ORIENT_WEST; break;
            case 2: player->data.orientation = ORIENT_EAST; break;
            case 3: player->data.orientation = ORIENT_NORTH; break;
            case 4: player->data.orientation = ORIENT_SOUTH; break;
        }
        if(player->data.orientation == o)
            player->data.moving = 1;
    }
    update_actor(player, delta);
    if(player->data.super_grid_x != 1 || player->data.super_grid_y != 1) {
        move_maps(1 - player->data.super_grid_x, 1 - player->data.super_grid_y);
        move_actor_maps(1 - player->data.super_grid_x, 1 - player->data.super_grid_y);
        player->data.super_grid_x = 1;
        player->data.super_grid_y = 1;
    }
    uint16_t view_width = 0;
    uint16_t view_height = 0;
    get_viewport_dimensions(&view_width, &view_height);
    update_view((player->data.position_x + (TILE_WIDTH * TILEMAP_DIMS) - (view_width / 2)), (player->data.position_y + TILE_HEIGHT * TILEMAP_DIMS - (view_height / 2)), 1);
}

void draw_player()
{
    draw_actor(player);
}

void destroy_player()
{
}
