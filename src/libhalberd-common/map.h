#ifndef MAP_H
#define MAP_H
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include "matrix.h"
#include "render_util.h"

#define TILEMAP_DIMS 128
#define TILEMAP_LAYERS 1

typedef struct tilemap
{
    uint16_t position_x;
    uint16_t position_y;
    
    GLuint tile_id_data[TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS];
    GLuint tile_set_data[TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS];
    uint8_t flags;
    GLuint tile_id_buffer;
    GLuint tile_set_buffer;
    uint8_t loaded;
}
tilemap;

typedef bool (*tilemap_loader)(FILE*, tilemap*, uint16_t x, uint16_t y);
typedef bool (*tilemap_saver)(FILE*, tilemap*, uint16_t x, uint16_t y);

char* get_map_name(uint32_t x, uint32_t y, const char* ext);
void set_map_name(const char* name);

void init_maps(tilemap_loader load_callback, tilemap_saver save_callback);
void destroy_maps();

void update_maps();
void draw_maps(mat4 transform);
void move_maps(int16_t x, int16_t y);

void clear_maps();
void save_tilemap_to_resource(const char* resource_location, const char* resource_name, tilemap* map, uint16_t x, uint16_t y);
void save_maps(const char* name);
void load_resource_to_tilemap(const char* resource_location, const char* resource_name, tilemap* map, uint16_t x, uint16_t y);
void load_maps(const char* name);

void get_true_tile_position(uint16_t* X, uint16_t* Y, int16_t* x, int16_t* y);
uint32_t get_tile(uint16_t X, uint16_t Y, int16_t x, int16_t y, uint16_t z);
void update_tile(uint16_t x, uint16_t y, uint16_t z, uint32_t tile, uint8_t map_id);
uint8_t get_solid(uint32_t tile);

#endif
