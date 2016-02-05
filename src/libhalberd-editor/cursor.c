#include "autotile.h"
#include "cursor.h"
#include "editor-render.h"
#include "stack.h"
#include "tile_util.h"
#include <dfgame-common.h>
#include <stdio.h>
#include <stdlib.h>

cursor main_cursor;
map_view mview;

void cursor_init()
{
    main_cursor.screen_x = 0;
    main_cursor.screen_y = 0;
    main_cursor.selected_tile_x = 0;
    main_cursor.selected_tile_y = 0;

    main_cursor.left_button = 0;
    main_cursor.right_button = 0;
    main_cursor.middle_button = 0;
    main_cursor.control_button = 0;

    main_cursor.selected_map = 0;
    main_cursor.set_id = 0;
    main_cursor.current_id = 1;

    mview.x = 0;
    mview.y = 0;
    mview.zoom = 1;
}

void cursor_set_position(int32_t x, int32_t y)
{
    main_cursor.screen_x = x;
    main_cursor.screen_y = y;
    main_cursor.selected_tile_x = ((main_cursor.screen_x + mview.x) / (TILE_WIDTH * mview.zoom));
    main_cursor.selected_tile_y = ((main_cursor.screen_y + mview.y) / (TILE_HEIGHT * mview.zoom));
    main_cursor.selected_map = (main_cursor.selected_tile_x / TILEMAP_DIMS) + (main_cursor.selected_tile_y / TILEMAP_DIMS * 3);
    main_cursor.selected_tile_x %= TILEMAP_DIMS;
    main_cursor.selected_tile_y %= TILEMAP_DIMS;
}

void cursor_set_id(uint32_t id)
{
    main_cursor.current_id = id;
}

void map_set_position(int32_t x, int32_t y)
{
    mview.x = x;
    mview.y = y;
}

void map_drag(int32_t x, int32_t y)
{
    mview.x -= x - main_cursor.screen_x;
    mview.y -= y - main_cursor.screen_y;
}

cursor cursor_get()
{
    return main_cursor;
}

map_view view_get()
{
    return mview;
}

bool cursor_in_bounds(int32_t true_x, int32_t true_y)
{
    return true_x >= 0 && true_y >= 0;
}

void cursor_place_tile()
{
    cursor_place_tile_id(main_cursor.current_id);
}

void cursor_place_tile_id(uint8_t tile)
{
    /*update_tile(main_cursor.selected_tile_x, main_cursor.selected_tile_y, 0, tile, main_cursor.selected_map);*/
    /*place_autotile(main_cursor.selected_map % 3, main_cursor.selected_map / 3, main_cursor.selected_tile_x, main_cursor.selected_tile_y, 0, tile);*/
    if(cursor_in_bounds(main_cursor.selected_tile_x, main_cursor.selected_tile_y))
        cursor_place_tile_at(tile, main_cursor.selected_tile_x, main_cursor.selected_tile_y);
}

void cursor_place_tile_at(uint8_t tile, int32_t x, int32_t y)
{
    update_tile(x, y, 0, tile, main_cursor.selected_map);
    /*place_autotile(main_cursor.selected_map % 3, main_cursor.selected_map / 3, x, y, 0, tile);*/
}

void cursor_place_line(int32_t x, int32_t y)
{
    cursor_place_line_id(main_cursor.current_id, x, y);
}

void cursor_place_line_id(uint8_t tile, int32_t x, int32_t y)
{
    // FIXME: Lines that go out of bounds of a map segment glitch out
    cursor prev = cursor_get();
    cursor_set_position(x, y);
    int x1 = prev.selected_tile_x;
    int y1 = prev.selected_tile_y;
    int x2 = main_cursor.selected_tile_x;
    int y2 = main_cursor.selected_tile_y;
    int dx = x2 - x1;
    int ymove = 1;
    if(dx < 0) {
        dx *= -1;
        int temp = x1;
        x1 = x2;
        x2 = temp;
        ymove *= -1;
    }
    int dy = y2 - y1;
    if(dy < 0) {
        dy *= -1;
        ymove *= -1;
    }
    int j = ymove == 1 ? y1 : y2;
    int d = 2*dy - dx;
    if(cursor_in_bounds(x1, j))
        cursor_place_tile_at(tile, x1, j);
    if(dx != 0) {
        for(int i = x1 + 1; i <= x2; ++i) {
            d += 2 * dy;
            if(d <= 0)
                if(cursor_in_bounds(i, j))
                    cursor_place_tile_at(tile, i, j);
            while(d > 0) {
                if(cursor_in_bounds(i, j))
                    cursor_place_tile_at(tile, i, j);
                j += ymove;
                d -= 2*dx;
            }
        }
    } else if(dy != 0) {
        for(int i = y1 + 1; i <= y2; ++i) {
            if(cursor_in_bounds(x1, i))
                cursor_place_tile_at(tile, x1, i);
        }
    } else {
        return;
    }
}

void cursor_flood_fill(uint8_t tile)
{
    int32_t p_x = main_cursor.selected_tile_x;
    int32_t p_y = main_cursor.selected_tile_y;
    uint8_t target = get_autotile(main_cursor.selected_map % 3, main_cursor.selected_map / 3, p_x, p_y, 0);
    if(target == 1)
        return;
    stack* cursor_stack = stack_make();
    int16_t* first_pair = calloc(sizeof(int16_t), 4);
    first_pair[0] = p_x;
    first_pair[1] = p_y;
    first_pair[2] = main_cursor.selected_map % 3;
    first_pair[3] = main_cursor.selected_map / 3;
    stack_push(cursor_stack, first_pair);
    uint16_t i = 0;
    uint16_t j = 1;
    do {
        ++i;
        int16_t* current_pair = stack_pop(cursor_stack, 0);
        nulltest(current_pair);
        uint16_t pxm = current_pair[2];
        uint16_t pym = current_pair[3];
        get_true_tile_position(&pxm, &pym, &current_pair[0], &current_pair[1]);
        place_autotile(pxm, pym, current_pair[0], current_pair[1], 0, 1);
        for(uint8_t i = 1; i < 7; i++) {
            if(i == 2 || i == 5)
                i++;
            uint8_t a = get_autotile(pxm, pym, current_pair[0] + surround_x_table[i], current_pair[1] + surround_y_table[i], 0);
            if(a != target) {
                continue;
            }
            ++j;
            int16_t* cursor_pair = calloc(sizeof(int16_t), 4);
            cursor_pair[0] = current_pair[0] + surround_x_table[i];
            cursor_pair[1] = current_pair[1] + surround_y_table[i];
            cursor_pair[2] = pxm;
            cursor_pair[3] = pym;
            stack_push(cursor_stack, cursor_pair);
        }
        --j;
        free(current_pair);
    } while(!stack_empty(cursor_stack));
}

void cursor_zoom(double input)
{
    mview.zoom += input / 10.0f;
    if(mview.zoom > 5)
        mview.zoom = 5;
    else if(mview.zoom < 0.01)
        mview.zoom = 0.01;
}
