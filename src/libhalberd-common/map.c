#include "actor.h"
#include "io_util.h"
#include "map.h"
#include "render_util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// Hidden variables
///////////////////////////////////////////////////////////////////////////////

tilemap maps[9]; int32_t central_pos_x = 1; int32_t central_pos_y = 1;
char* mapname = 0;
tilemap_loader load_callback_fn;
tilemap_saver save_callback_fn;

///////////////////////////////////////////////////////////////////////////////
// Hidden functions
///////////////////////////////////////////////////////////////////////////////

void mapcp(uint8_t from, uint8_t to)
{
    if(from > 8 || to > 8)
        return;
    maps[to].loaded = maps[from].loaded;
    GLuint id_temp = maps[to].tile_id_buffer;
    GLuint set_temp = maps[to].tile_set_buffer;
    GLuint conv_temp = maps[to].tile_id_conversion_buffer;
    maps[to].tile_id_buffer = maps[from].tile_id_buffer;
    maps[to].tile_set_buffer = maps[from].tile_set_buffer;
    maps[to].tile_id_conversion_buffer = maps[from].tile_id_conversion_buffer;
    maps[from].tile_id_buffer = id_temp;
    maps[from].tile_set_buffer = set_temp;
    maps[from].tile_id_conversion_buffer = conv_temp;
    memcpy(maps[to].tile_id_data, maps[from].tile_id_data, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint));
    memcpy(maps[to].tile_set_data, maps[from].tile_set_data, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint));
    maps[to].tileset_count = maps[from].tileset_count;
    memcpy(maps[to].tileset_names, maps[from].tileset_names, maps[from].tileset_count * sizeof(char*));
    memcpy(maps[to].tileset_ids, maps[from].tileset_ids, maps[from].tileset_count * sizeof(uint8_t));
}

void save_tilemap_to_resource(const char* resource_location, const char* resource_name, tilemap* map, uint16_t x, uint16_t y)
{
    FILE* file = load_resource_file(resource_location, resource_name, "r+!");
    save_callback_fn(file, map, x, y);
    fclose(file);
}

void load_resource_to_tilemap(const char* resource_location, const char* resource_name, tilemap* map, uint16_t x, uint16_t y)
{
    FILE* file = load_resource_file(resource_location, resource_name, "r");
    load_callback_fn(file, map, x, y);
    fclose(file);
}

///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

char* get_map_name(uint32_t x, uint32_t y, const char* ext)
{
    char* filename = calloc(strlen(mapname) + strlen(ext) + 19, sizeof(char));
    sprintf(filename, "%s%dx%d.%s", mapname, x, y, ext);
    return filename;
}

void set_map_name(const char* name)
{
    if(mapname)
        free(mapname);
    mapname = strdup(name);
}

void init_maps(tilemap_loader load_callback, tilemap_saver save_callback)
{
    load_callback_fn = load_callback;
    save_callback_fn = save_callback;

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

void destroy_maps()
{
    for(uint8_t i = 0; i < 9; ++i) {
        glDeleteBuffers(1, &maps[i].tile_id_buffer);
        glDeleteBuffers(1, &maps[i].tile_set_buffer);
        for(uint8_t j = 0; i < maps[j].tileset_count; ++i) {
            free(maps[i].tileset_names[j]);
        }
        free(maps[i].tileset_ids);
        free(maps[i].tileset_names);
        maps[i].tileset_count = 0;
        maps[i].loaded = 0;
    }
}

void update_maps()
{
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
                // TODO: Replace this
                /*load_map(filename, &maps[i * 3 + 1 - x]);*/
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
                // TODO: Replace this
                /*load_map(filename, &maps[i + 3 - (y * 3)]);*/
                glBindBuffer(GL_ARRAY_BUFFER, maps[i + 3 - (y * 3)].tile_id_buffer);
                glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i + 3 - (y * 3)].tile_id_data, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, maps[i + 3 - (y * 3)].tile_set_buffer);
                glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i].tile_set_data, GL_DYNAMIC_DRAW);
                free(filename);
            }
        }
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

void save_maps_to_resource(const char* resource_location, const char* resource_name)
{
    if(mapname)
        free(mapname);
    mapname = strdup(resource_name);
    for(uint8_t i = 0; i < 9; ++i) {
        if(maps[i].loaded) {
            save_tilemap_to_resource(resource_location, resource_name, &maps[i], i % 3, i / 3);
        }
    }
}

void load_maps_from_resource(const char* resource_location, const char* resource_name)
{
    if(mapname)
        free(mapname);
    mapname = strdup(resource_name);
    for(uint8_t i = 0; i < 9; ++i) {
        load_resource_to_tilemap(resource_location, resource_name, &maps[i], i % 3, i / 3);
        glBindBuffer(GL_ARRAY_BUFFER, maps[i].tile_id_buffer);
        glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i].tile_id_data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, maps[i].tile_set_buffer);
        glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * sizeof(GLuint), maps[i].tile_set_data, GL_DYNAMIC_DRAW);
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

void update_tile(uint16_t x, uint16_t y, uint16_t z, uint32_t tile, uint8_t map_id)
{
    // TODO: This shouldn't be hardcoded
    maps[map_id].tile_id_data[x + (TILEMAP_DIMS * y) + (TILEMAP_DIMS * TILEMAP_DIMS * z)] = tile;
    glBindBuffer(GL_ARRAY_BUFFER, maps[map_id].tile_id_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLuint) * (x + (TILEMAP_DIMS * y) + (TILEMAP_DIMS * TILEMAP_DIMS * z)), sizeof(GLuint), &maps[map_id].tile_id_data[x + (TILEMAP_DIMS * y) + (TILEMAP_DIMS * TILEMAP_DIMS * z)]);
}

int16_t add_tileset_from_resource(const char* resource_location, const char* resource_name, tilemap* map)
{
    uint8_t id = get_tileset_id(resource_location, resource_name);
    for(uint8_t i = 0; i < map->tileset_count; ++i) {
        if(map->tileset_ids[i] == id)
            return i;
    }
    map->tileset_count++;
    map->tileset_ids = realloc(map->tileset_ids, map->tileset_count * sizeof(uint8_t));
    map->tileset_ids[map->tileset_count - 1] = id;
    map->tileset_names = realloc(maps->tileset_names, map->tileset_count * sizeof(char*));
    map->tileset_names[map->tileset_count - 1] = strdup(resource_name);
    map->tileset_paths = realloc(maps->tileset_paths, map->tileset_count * sizeof(char*));
    if(resource_location != 0)
        map->tileset_paths[map->tileset_count - 1] = strdup(resource_location);
    else
        map->tileset_paths[map->tileset_count - 1] = 0;

    return map->tileset_count;
}

int16_t add_tileset_to_all_from_resource(const char* resource_location, const char* resource_name)
{
    int16_t id = -1;
    for(uint8_t i = 0; i < 9; ++i) {
        id = add_tileset_from_resource(resource_location, resource_name, &maps[i]);
    }

    return id;
}

uint8_t get_tileset_count()
{
    return maps[0].tileset_count;
}

const char* get_tileset_name(uint8_t id)
{
    return maps[0].tileset_names[id];
}

const char* get_tileset_location(uint8_t id)
{
    return maps[0].tileset_paths[id];
}

uint8_t get_solid(uint32_t tile)
{
    return tile != 0;
}
