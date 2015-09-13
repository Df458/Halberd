#include <stdlib.h>

#include "settings.h"

tileset_data* tiles = 0;

void init_settings()
{
    tiles = malloc(sizeof(tileset_data*));
    tiles->width = 32;
    tiles->height = 32;
    glGenTextures(1, &tiles->texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, tiles->texture);
}

uint8_t load_settings()
{
    // TODO: Load tile settings
    return 1;
}

void cleanup_settings()
{
    glDeleteTextures(1, &tiles->texture);
    free(tiles);
}

const tileset_data* tile_settings() { return tiles; }
