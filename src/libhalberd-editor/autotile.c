#include "autotile.h"
#include "io_util.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
automap automaps[9];

#define AUTOMAP_FILE_VERSION "0.1.0"

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

void clear_automaps()
{
    for(int i = 0; i < 9; ++i) {
        memset(automaps[i].tile_data, 0, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS);
    }
}

void init_automaps()
{
    init_maps(automap_loader, automap_saver);
    clear_automaps();
}

bool automap_loader(FILE* infile, tilemap* map, uint16_t x, uint16_t y)
{
    rewind(infile);
    size_t header_size = 0;
    size_t chunk_size = TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS;
    uint16_t width = 0;
    uint16_t height = 0;
    fread(&header_size, sizeof(size_t), 1, infile);
    fread(&width, sizeof(uint16_t), 1, infile);
    fread(&height, sizeof(uint16_t), 1, infile);
    fread(&(map->tileset_count), sizeof(uint8_t), 1, infile);
    for(int i = 0; i < map->tileset_count; ++i) {
        add_tileset_from_resource(read_string_from_file(infile), read_string_from_file(infile), map);
    }

    fseek(infile, header_size + (x + y * width) * chunk_size * 2, SEEK_SET);
    fread(map->tile_id_data, sizeof(uint8_t), chunk_size, infile);
    fread(automaps[x + y * 3].tile_data, sizeof(uint8_t), chunk_size, infile);

    map->loaded = 1;

    return true;
}

bool automap_saver(FILE* infile, tilemap* map, uint16_t x, uint16_t y)
{
    rewind(infile);
    size_t header_size = sizeof(uint16_t) * 2;
    size_t chunk_size = TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS;
    uint16_t width = 3;
    uint16_t height = 3;
    fwrite(&header_size, sizeof(size_t), 1, infile);
    fwrite(&width, sizeof(uint16_t), 1, infile);
    fwrite(&height, sizeof(uint16_t), 1, infile);
    fwrite(&(map->tileset_count), sizeof(uint8_t), 1, infile);

    fseek(infile, header_size + (x + y * width) * chunk_size * 2, SEEK_SET);
    fwrite(map->tile_id_data, sizeof(uint8_t), chunk_size, infile);
    fwrite(automaps[x + y * 3].tile_data, sizeof(uint8_t), chunk_size, infile);

    return true;
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
