#ifndef AUTOTILE_H
#define AUTOTILE_H
#include "map.h"

typedef struct automap
{
    tilemap* map;
    uint8_t tile_data[TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS];
}
automap;

void init_automaps();
void clear_automaps();

bool automap_loader(FILE* infile, tilemap* map, uint16_t x, uint16_t y);
bool automap_saver(FILE* infile, tilemap* map, uint16_t x, uint16_t y);

void place_autotile(uint16_t X, uint16_t Y, uint16_t x, uint16_t y, uint16_t z, uint8_t tile);
uint8_t get_autotile(uint16_t X, uint16_t Y, int16_t x, int16_t y, uint16_t z);

#endif
