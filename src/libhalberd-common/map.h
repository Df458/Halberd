#ifndef MAP_H
#define MAP_H
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include <dfgame-common.h>
#include "render_util.h"

#define TILEMAP_DIMS 128
#define TILEMAP_LAYERS 1

typedef struct tilemap
{
    // General Map Data
    uint16_t position_x;
    uint16_t position_y;
    uint8_t flags;
    uint8_t loaded;
    
    // Tile Data
    GLuint tile_id_data[TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS];
    GLuint tile_set_data[TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS];

    // Tileset Information
    uint8_t  tileset_count;
    char**   tileset_names;
    char**   tileset_paths;
    uint8_t* tileset_ids;

    // Render Buffers
    GLuint tile_id_buffer;
    GLuint tile_set_buffer;
    // This buffer holds tileset_ids, and is used to convert from the id in
    // tile_id_buffer to the actual id of the loaded tileset
    GLuint tile_id_conversion_buffer;
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
void save_maps_to_resource(const char* resource_location, const char* resource_name);
void load_maps_from_resource(const char* resource_location, const char* resource_name);

void get_true_tile_position(uint16_t* X, uint16_t* Y, int16_t* x, int16_t* y);
uint32_t get_tile(uint16_t X, uint16_t Y, int16_t x, int16_t y, uint16_t z);
void update_tile(uint16_t x, uint16_t y, uint16_t z, uint32_t tile, uint8_t map_id);
int16_t add_tileset_from_resource(const char* resource_location, const char* resource_name, tilemap* map);
int16_t add_tileset_to_all_from_resource(const char* resource_location, const char* resource_name);
uint8_t get_tileset_count();
// TODO: Replace this with a way of getting a list of loaded tilesets
const char* get_tileset_name(uint8_t id);
const char* get_tileset_location(uint8_t id);
uint8_t get_solid(uint32_t tile);

#endif
