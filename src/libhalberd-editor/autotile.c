#include "autotile.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
automap automaps[9];

static const uint8_t autotile_lookup[] =
{
    0b11111111,
    0b01111111,
    0b11011111,
    0b01011111,
    0b00011111,
    
    0b11110111,
    0b01110111,
    0b11010111,
    0b01010111,
    0b00010111,
    0b11000111,
    0b01000111,
    0b00000111,

    0b11111101,
    0b01111101,
    0b11011101,
    0b01011101,
    0b00011101,
    0b11110101,
    0b01110101,
    0b11010101,
    0b01010101,
    0b00010101,
    0b11000101,
    0b01000101,
    0b00000101,
    0b11110001,
    0b01110001,
    0b11010001,
    0b01010001,
    0b00010001,
    0b11000001,
    0b01000001,
    0b00000001,

    0b01111100,
    0b01011100,
    0b00011100,
    0b01110100,
    0b01010100,
    0b00010100,
    0b01000100,
    0b00000100,
    0b01110000,
    0b01010000,
    0b00010000,
    0b01000000,
    0b00000000,
};

void update_static(uint16_t X, uint16_t Y, int16_t x, int16_t y, int16_t z)
{
    get_true_tile_position(&X, &Y, &x, &y);
    uint8_t tile = automaps[Y * 3 + X].tile_data[x + (TILEMAP_DIMS * y) + (TILEMAP_DIMS * z)];
    if(tile == 0)
        return;
    uint8_t map = 0;
    if(get_autotile(X, Y, x - 1, y - 1, z) == tile)
        map += 0b10000000;
    if(get_autotile(X, Y, x, y - 1, z) == tile)
        map += 0b01000000;
    if(get_autotile(X, Y, x + 1, y - 1, z) == tile)
        map += 0b00100000;
    if(get_autotile(X, Y, x + 1, y, z) == tile)
        map += 0b00010000;
    if(get_autotile(X, Y, x + 1, y + 1, z) == tile)
        map += 0b00001000;
    if(get_autotile(X, Y, x, y + 1, z) == tile)
        map += 0b00000100;
    if(get_autotile(X, Y, x - 1, y + 1, z) == tile)
        map += 0b00000010;
    if(get_autotile(X, Y, x - 1, y, z) == tile)
        map += 0b00000001;

    int i;
    for(i = 0; i < 47; ++i) {
        if((autotile_lookup[i] & map) == autotile_lookup[i])
            break;
    }
    if(i == 47)
        i = -1;

    // Place i on map
    update_tile(x, y, z, i + 1, Y * 3 + X);
}

void load_automap(const char* path, uint8_t map_id)
{
    unsigned char* data = load_file(path);

    int i;
    for(i = 0; i < TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS; ++i) {
        automaps[map_id].tile_data[i] = data[i];
    }
    /*for(int i = 0; i < TILEMAP_DIMS; ++i) {*/
        /*for(int j = 0; j < TILEMAP_DIMS; ++j) {*/
            /*for(int k = 0; k < TILEMAP_LAYERS; ++k) {*/
                /*update_static(map_id % 3, map_id / 3, j, i, k);*/
            /*}*/
        /*}*/
    /*}*/
}

void clear_automaps()
{
    for(int i = 0; i < 9; ++i) {
        memset(automaps[i].tile_data, 0, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS);
    }
}

void init_automaps()
{
    init_maps();
    clear_automaps();
}

void load_automaps(const char* name)
{
    set_map_name(name);
    /*load_maps(name);*/

    for(uint8_t i = 0; i < 9; ++i) {
        char* filename = get_map_name(i % 3, i / 3, "automap");
        load_automap(filename, i);
        free(filename);
    }
}

void save_automap(const char* path, automap* map)
{
    FILE* file = ex_fopen(path, "wb");
    if(!file)
        return NULL;
    // TODO: This is slow. Try something quicker.
    int i;
    for(i = 0; i < TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS; ++i) {
        fputc(map->tile_data[i], file);
    }
    fputc(0, file);
    fclose(file);
}

void save_automaps()
{
    int i;
    for(i = 0; i < 9; ++i) {
        char* filename = get_map_name(i % 3, i / 3, "automap");
        save_automap(filename, &automaps[i]);
        free(filename);
    }
}

void save_automaps_named(const char* name)
{
    set_map_name(name);
    save_automaps();
}

void place_autotile(uint16_t X, uint16_t Y, uint16_t x, uint16_t y, uint16_t z, uint8_t tile)
{
    automaps[Y * 3 + X].tile_data[x + (TILEMAP_DIMS * y) + (TILEMAP_DIMS * z)] = tile;
    int i,j;
    for(int i = y - 1; i <= y + 1; ++i) {
        for(int j = x - 1; j <= x + 1; ++j) {
            update_static(X, Y, j, i, z);
        }
    }
}

uint8_t get_autotile(uint16_t X, uint16_t Y, int16_t x, int16_t y, uint16_t z)
{
    get_true_tile_position(&X, &Y, &x, &y);
    if(z >= TILEMAP_LAYERS || z < 0)
        return 0;
    /*if(!automaps[Y * 3 + X].loaded)*/
        /*return 0;*/
    return automaps[Y * 3 + X].tile_data[x + (y*TILEMAP_DIMS) + (z*TILEMAP_DIMS*TILEMAP_DIMS)];
}
