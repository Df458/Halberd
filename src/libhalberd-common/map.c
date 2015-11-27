#include "actor.h"
#include "io.h"
#include "map.h"
#include "render_util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
tilemap maps[9]; int32_t central_pos_x = 1; int32_t central_pos_y = 1;
char* mapname = "";

void init_maps()
{
    int i;
    for(i = 0; i < 9; ++i) {
        maps[i].flags = 0;
        maps[i].loaded = 1;
        maps[i].position_x = i % 3;
        maps[i].position_y = i / 3;
        glGenBuffers(1, &maps[i].tile_id_buffer);
        glGenBuffers(1, &maps[i].tile_set_buffer);
        checkGLError();
    }
    clear_maps();
    checkGLError();
}

void clear_maps()
{
    for(int i = 0; i < 9; ++i) {
        memset(maps[i].tile_id_data, 0, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint));
        memset(maps[i].tile_set_data, 0, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint));
        glBindBuffer(GL_ARRAY_BUFFER, maps[i].tile_id_buffer);
        glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i].tile_id_data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, maps[i].tile_set_buffer);
        glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i].tile_set_data, GL_DYNAMIC_DRAW);
    }
}

char* get_map_name(uint32_t x, uint32_t y, const char* ext)
{
    char* filename = calloc(strlen(mapname) + strlen(ext) + 19, sizeof(char));
    sprintf(filename, "%s%dx%d.%s", mapname, x, y, ext);
    return filename;
}

void set_map_name(const char* name)
{
    // TODO: SET NAME
}

void load_maps(const char* name)
{
    mapname = name;
    for(uint8_t i = 0; i < 9; ++i) {
        char* filename = get_map_name(i % 3, i / 3, "map");
        load_map(filename, &maps[i]);
        glBindBuffer(GL_ARRAY_BUFFER, maps[i].tile_id_buffer);
        glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i].tile_id_data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, maps[i].tile_set_buffer);
        glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i].tile_set_data, GL_DYNAMIC_DRAW);
        free(filename);
    }
}

void update_maps()
{
}

void update_tile(uint16_t x, uint16_t y, uint16_t z, uint32_t tile, uint8_t map_id)
{
    // TODO: This shouldn't be hardcoded
    maps[map_id].tile_id_data[x + (TILEMAP_DIMS * y) + (TILEMAP_DIMS * TILEMAP_DIMS * z)] = tile;
    glBindBuffer(GL_ARRAY_BUFFER, maps[map_id].tile_id_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLuint) * (x + (TILEMAP_DIMS * y) + (TILEMAP_DIMS * TILEMAP_DIMS * z)), sizeof(GLuint), &maps[map_id].tile_id_data[x + (TILEMAP_DIMS * y) + (TILEMAP_DIMS * TILEMAP_DIMS * z)]);
}

void draw_maps(mat4 transform)
{
    int i, x, y, z;
    glDisable(GL_DEPTH_TEST);
    for(i = 0; i < 9; ++i) {
        if(maps[i].loaded)
            draw_tiles(maps[i].tile_id_buffer, maps[i].tile_set_buffer, transform, maps[i].position_x, maps[i].position_y);
    }
    glEnable(GL_DEPTH_TEST);
}

void save_maps()
{
    int i;
    for(i = 0; i < 9; ++i) {
        if(maps[i].loaded) {
            char* filename = get_map_name(i % 3, i / 3, "map");
            save_map(filename, &maps[i]);
            free(filename);
        }
    }
}

void get_true_tile_position(uint16_t* X, uint16_t* Y, int16_t* x, int16_t* y)
{
    while(*x >= TILEMAP_DIMS) {
        *x -= TILEMAP_DIMS;
        *X += 1;
    }
    while(*y >= TILEMAP_DIMS) {
        *y -= TILEMAP_DIMS;
        *Y += 1;
    }
    while(*x < 0) {
        *x += TILEMAP_DIMS;
        *X -= 1;
    }
    while(*y < 0) {
        *y += TILEMAP_DIMS;
        *Y -= 1;
    }
    while(*X >= 3) {
        *X -= 3;
    }
    while(*Y >= 3) {
        *Y -= 3;
    }
    while(*X < 0) {
        *X += 3;
    }
    while(*Y < 0) {
        *Y += 3;
    }
}

uint32_t get_tile(uint16_t X, uint16_t Y, int16_t x, int16_t y, uint16_t z)
{
    get_true_tile_position(&X, &Y, &x, &y);
    if(z >= TILEMAP_LAYERS || z < 0)
        return 0;
    if(!maps[Y * 3 + X].loaded)
        return 0;
    return maps[Y * 3 + X].tile_id_data[x + (y*TILEMAP_DIMS) + (z*TILEMAP_DIMS*TILEMAP_DIMS)];
}

uint8_t get_solid(uint32_t tile)
{
    return tile != 0;
}

void destroy_maps()
{
    for(uint8_t i = 0; i < 9; ++i) {
        glDeleteBuffers(1, &maps[i].tile_id_buffer);
        glDeleteBuffers(1, &maps[i].tile_set_buffer);
        maps[i].loaded = 0;
    }
}

void mapcp(uint8_t from, uint8_t to)
{
    if(from > 8 || to > 8)
        return;
    /*maps[from].position_x = maps[to].position_x;*/
    /*maps[from].position_y = maps[to].position_y;*/
    /*memcpy(&maps[to], &maps[from], sizeof(tilemap));*/
    maps[to].loaded = maps[from].loaded;
    GLuint id_temp = maps[to].tile_id_buffer;
    GLuint set_temp = maps[to].tile_set_buffer;
    maps[to].tile_id_buffer = maps[from].tile_id_buffer;
    maps[to].tile_set_buffer = maps[from].tile_set_buffer;
    maps[from].tile_id_buffer = id_temp;
    maps[from].tile_set_buffer = set_temp;
    memcpy(maps[to].tile_id_data, maps[from].tile_id_data, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint));
    memcpy(maps[to].tile_set_data, maps[from].tile_set_data, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint));
    /*maps[to].position_x = maps[from].position_x;*/
    /*maps[to].position_y = maps[from].position_y;*/
}

void move_maps(int16_t x, int16_t y)
{
    if(x == 0 && y == 0)
        return;

    /*fprintf(stderr, "Prev: %dx%d\nNext: %dx%d\n", central_pos_x, central_pos_y, central_pos_x - x, central_pos_y - y);*/
    central_pos_x -= x;
    central_pos_y -= y;

    int8_t xmin = 0;
    int8_t xmax = 3;
    int8_t xincr = 1;
    int8_t ymin = 0;
    int8_t ymax = 3;
    int8_t yincr = 1;

    if(x > 0) {
        xmin = 2 - x;
        xmax = -1;
        xincr = -1;
    } else if(x < 0) {
        xmin -= x;
    }
    if(y > 0) {
        ymin = 3 - y;
        ymax = -1;
        yincr = -1;
    } else if(y < 0) {
        ymin -= y;
    }
    for(int8_t i = ymin; i != ymax; i += yincr) {
        for(int8_t j = xmin; j != xmax; j += xincr) {
            mapcp(i * 3 + j, (i * 3 + j) + (y * 3 + x));
        }
    }

    if(x != 0)
        for(uint8_t i = 0; i < 3; ++i) {
            maps[i * 3 + (1 - x)].loaded = 0;
        }
    if(y != 0)
        for(uint8_t i = 0; i < 3; ++i) {
            maps[i + ((1 - y) * 3)].loaded = 0;
        }
    if(x != 0)
        for(uint8_t i = 0; i < 3; ++i) {
            if(central_pos_x - x < 3 && central_pos_x - x > -1) {
                char* filename = get_map_name(central_pos_x - x, i, "map");
                load_map(filename, &maps[i * 3 + 1 - x]);
                glBindBuffer(GL_ARRAY_BUFFER, maps[i * 3 + 1 - x].tile_id_buffer);
                glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i * 3 + 1 - x].tile_id_data, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, maps[i * 3 + 1 - x].tile_set_buffer);
                glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i * 3 + 1 - x].tile_set_data, GL_DYNAMIC_DRAW);
                free(filename);
            }
        }
    if(y != 0)
        for(uint8_t i = 0; i < 3; ++i) {
            if(central_pos_y - y < 3 && central_pos_y - y > -1) {
                char* filename = get_map_name(i, central_pos_y - y, "map");
                load_map(filename, &maps[i + 3 - (y * 3)]);
                glBindBuffer(GL_ARRAY_BUFFER, maps[i + 3 - (y * 3)].tile_id_buffer);
                glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i + 3 - (y * 3)].tile_id_data, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, maps[i + 3 - (y * 3)].tile_set_buffer);
                glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i].tile_set_data, GL_DYNAMIC_DRAW);
                free(filename);
            }
        }
}
