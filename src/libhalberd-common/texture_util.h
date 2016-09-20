#ifndef TEXTURE_UTIL_H
#define TEXTURE_UTIL_H
#include <dfgame-common.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <inttypes.h>
#include <stdbool.h>

#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define MAX_LOADED_TILESETS 32

#if MAX_LOADED_TILESETS > 256
#error "MAX_LOADED_TILESETS cannot be higher than 256 due to OpenGL limitations"
#endif

//-----------------------------------------------------------------------------
// Struct Section
//-----------------------------------------------------------------------------

typedef struct spritefont
{
    texture* texture_data;
    uint16_t glyph_width;
    uint16_t glyph_height;
}
spritefont;

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
 * Creates an empty spritefont
 */
spritefont* create_spritefont(uint16_t w, uint16_t h);

/*!
 * Frees a spritefont and its resources
 */
void destroy_spritefont(spritefont* fnt);

/*!
 * This loads a spritefont.
 * See get_extended_resource_path(io_util.h) for usage details
 */
spritefont* load_resource_to_spritefont(const char* resource_location, const char* resource_name);

/*!
 * This saves a spritefont.
 * See get_extended_resource_path(io_util.h) for usage details
 */
bool save_spritefont_to_resource(spritefont* fnt, const char* resource_location, const char* resource_name);


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

int16_t get_tileset_id(const char* resource_location, const char* resource_name);
tileset* get_tileset_from_id(uint8_t id);

// TODO: Find a more graceful way to pass this around
GLuint get_tileset_texture();

#endif
