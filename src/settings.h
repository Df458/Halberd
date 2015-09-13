#ifndef SETTINGS_H
#define SETTINGS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <inttypes.h>

typedef struct tileset_data
{
    GLuint texture;
    uint8_t width;
    uint8_t height;
} tileset_data;

void init_settings();
uint8_t load_settings();
void cleanup_settings();

const tileset_data* tile_settings();

#endif
