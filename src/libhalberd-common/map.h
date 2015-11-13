#ifndef MAP_H
#define MAP_H
#include <inttypes.h>

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
} tilemap;

void init_maps();
void update_maps();
void clear_maps();
char* get_map_name(uint32_t x, uint32_t y, const char* ext);
void load_maps(const char* name);
void update_tile(uint16_t x, uint16_t y, uint16_t z, uint32_t tile, uint8_t map_id);
void draw_maps(mat4 transform);
void destroy_maps();
void save_maps();
void move_maps(int16_t x, int16_t y);
void get_true_tile_position(uint16_t* X, uint16_t* Y, int16_t* x, int16_t* y);
uint32_t get_tile(uint16_t X, uint16_t Y, int16_t x, int16_t y, uint16_t z);
uint8_t get_solid(uint32_t tile);

#endif
