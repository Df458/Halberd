#ifndef TEXTURE_UTIL_H
#define TEXTURE_UTIL_H
#include <GL/glew.h>
#include <GL/gl.h>
#include <inttypes.h>
#include <stdbool.h>

#include "dfgame-common.h"

#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define MAX_LOADED_TILESETS 32

#if MAX_LOADED_TILESETS > 256
#error "MAX_LOADED_TILESETS cannot be higher than 256 due to OpenGL limitations"
#endif

//-----------------------------------------------------------------------------
// Struct Section
//-----------------------------------------------------------------------------

typedef struct font
{
    texture* texture_data;
    uint16_t glyph_width;
    uint16_t glyph_height;
}
font;

struct animation
{
    uint8_t orients; // 0 for no(1 orient), 1 for 4(grid orient), 2 for(full orient)
    uint16_t dimensions_x;
    uint16_t dimensions_y;
    int16_t origin_x;
    int16_t origin_y;
    int8_t length;
    uint8_t delay;
    bool loop;
    bool play;
    float offset_x; // offset is for the position in the texture atlas
    float offset_y;
    float size_x; // size is for the size in the texture atlas
    float size_y;
    char* handle;
};

typedef struct sprite
{
    texture* atlas;
    struct animation* animations;
    uint8_t animation_count;
}
sprite;

typedef struct ui_box
{
    texture* texture_data;
    GLuint uv_buffer;

    uint16_t w;
    uint16_t h;

    uint16_t border_w;
    uint16_t border_h;
}
ui_box;

typedef struct tileset
{
    int16_t layer;
    uint8_t solid[1024];

    char* resource_location;
    char* resource_name;
}
tileset;

//-----------------------------------------------------------------------------
// Loading section
//-----------------------------------------------------------------------------

// TODO: Use XML-based font files
/*!
 * Creates an empty font
 */
font* create_font(uint16_t w, uint16_t h);

/*!
 * Frees a font and its resources
 */
void destroy_font(font* fnt);

/*!
 * This loads a font.
 * See get_extended_resource_path(io_util.h) for usage details
 */
font* load_resource_to_font(const char* resource_location, const char* resource_name);

/*!
 * This saves a font.
 * See get_extended_resource_path(io_util.h) for usage details
 */
bool save_font_to_resource(font* fnt, const char* resource_location, const char* resource_name);


/*!
 * Creates an empty sprite
 */
sprite* create_sprite();

/*!
 * Frees a sprite and its resources
 */
void destroy_sprite(sprite* fnt);

/*!
 * This loads a sprite file, and constructs a texture atlas from the
 * necessary spritesheets.
 * See get_extended_resource_path(io_util.h) for usage details
 */
sprite* load_resource_to_sprite(const char* resource_location, const char* resource_name);

/*!
 * This saves a sprite.
 * See get_extended_resource_path(io_util.h) for usage details
 */
bool save_sprite_to_resource(sprite* fnt, const char* resource_location, const char* resource_name);


/*!
 * Creates an empty tileset
 */
tileset* create_tileset();

/*!
 * Frees a tileset and its resources
 */
void destroy_tileset(tileset* fnt);

/*!
 * This loads a tileset file, and constructs a texture atlas from the
 * necessary tilesetsheets.
 * See get_extended_resource_path(io_util.h) for usage details
 */
tileset* load_resource_to_tileset(const char* resource_location, const char* resource_name);

/*!
 * This saves a tileset.
 * See get_extended_resource_path(io_util.h) for usage details
 */
bool save_tileset_to_resource(tileset* set, const char* resource_location, const char* resource_name);

int8_t index_by_handle(sprite* spr, const char* handle);
int16_t get_tileset_id(const char* resource_location, const char* resource_name);
tileset* get_tileset_from_id(uint8_t id);

// TODO: Find a more graceful way to pass this around
GLuint get_tileset_texture();

#endif
