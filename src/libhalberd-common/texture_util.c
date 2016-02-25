#include "texture_util.h"
#include "render_util.h"

#include <png.h>
#include <stdlib.h>
#include <dfgame-common.h>

///////////////////////////////////////////////////////////////////////////////
// Hidden data
///////////////////////////////////////////////////////////////////////////////

tileset tilesets[MAX_LOADED_TILESETS];
uint8_t loaded_tilesets = 0;
GLuint tile_buffer = 0;

///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

font* create_font(uint16_t w, uint16_t h)
{
	font* font_data = malloc(sizeof(font));
    font_data->glyph_width = w;
    font_data->glyph_width = h;
    font_data->texture_data = create_texture(font_data->glyph_width * 16, font_data->glyph_height * 16);

    return font_data;
}

void destroy_font(font* fnt)
{
    destroy_texture(fnt->texture_data);
    free(fnt);
}

font* load_resource_to_font(const char* resource_location, const char* resource_name)
{
    font* new_font         = malloc(sizeof(font));
    new_font->texture_data = load_resource_to_texture(resource_location, resource_name);
    new_font->glyph_width  = new_font->texture_data->width  / 16;
    new_font->glyph_width  = new_font->texture_data->height / 16;

    return new_font;
}

bool save_font_to_resource(font* fnt, const char* resource_location, const char* resource_name)
{
    stub(false);
}

tileset* create_tileset()
{
    uint8_t i;
    for(i = 0; i < MAX_LOADED_TILESETS; ++i) {
        if(tilesets[i].layer == -1 || i >= loaded_tilesets)
            break;
    }
    if(i > loaded_tilesets && i < MAX_LOADED_TILESETS)
        loaded_tilesets++;
    else if(i == MAX_LOADED_TILESETS) {
        error("Can't create tileset: All tileset slots are full.");
        return 0;
    }

    tileset* set = &tilesets[i];
    set->layer = i;
    return set;
}

void destroy_tileset(tileset* set)
{
    if(set->resource_location)
        free(set->resource_location);
    free(set->resource_name);
    set->layer = -1;
}

// TODO: Add support for paths in tileset files
tileset* load_resource_to_tileset(const char* resource_location, const char* resource_name)
{
    info("Loading tileset %s", resource_name);
    tileset* new_tileset = create_tileset();
    if(!new_tileset)
        return 0;
    uint16_t w, h;
    uint8_t* image_data = load_resource_to_texture_buffer(resource_location, resource_name, &w, &h);
    // TODO: Make this more flexible about sizing
    if(!image_data || w != TILE_WIDTH * 32 || h != TILE_HEIGHT * 32) {
        if(image_data) {
            free(image_data);
            error("Tileset dimensions are incorrect. (expected %ux%u, got %ux%u)", TILE_WIDTH * 32, TILE_HEIGHT * 32, w, h);
        } else
            error("Failed to load tileset texture data.");
        new_tileset->layer = -1;
        return 0;
    }
    if(resource_location)
        new_tileset->resource_location = strdup(resource_location);
    else
        new_tileset->resource_location = 0;
    new_tileset->resource_name = strdup(resource_name);

    if(tile_buffer == 0) {
        glGenTextures(1, &tile_buffer);
        glBindTexture(GL_TEXTURE_2D_ARRAY, tile_buffer);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, TILE_WIDTH * 32, TILE_HEIGHT * 32, MAX_LOADED_TILESETS);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        if(checkGLError()) {
            fatal("Failed to initialize the tileset buffer");
        }
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, tile_buffer);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, new_tileset->layer, TILE_WIDTH * 32, TILE_HEIGHT * 32, 1, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    checkGLError();

    free(image_data);
    // TODO: Set solids
    info("Loaded tileset with ID %d", new_tileset->layer);
    return new_tileset;
}

bool save_tileset_to_resource(tileset* spr, const char* resource_location, const char* resource_name)
{
    stub(false);
}

int16_t get_tileset_id(const char* resource_location, const char* resource_name)
{
    uint8_t i;
    for(i = 0; i < loaded_tilesets; ++i)
        if(tilesets[i].layer != -1 && resource_eq(resource_location, resource_name, tilesets[i].resource_location, tilesets[i].resource_name))
            return i;

    tileset* set = load_resource_to_tileset(resource_location, resource_name);
    if(!set)
        return -1;

    return loaded_tilesets - 1;
}

tileset* get_tileset_from_id(uint8_t id)
{
    if(id > loaded_tilesets || tilesets[id].layer == -1) {
        error("No tileset found at ID %u", id);
        return 0;
    }
    return &tilesets[id];
}

GLuint get_tileset_texture()
{
    if(tile_buffer == 0) {
        glGenTextures(1, &tile_buffer);
        glBindTexture(GL_TEXTURE_2D_ARRAY, tile_buffer);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, TILE_WIDTH * 32, TILE_HEIGHT * 32, MAX_LOADED_TILESETS);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        if(checkGLError()) {
            fatal("Failed to initialize the tileset buffer");
        }
    }
    return tile_buffer;
}
