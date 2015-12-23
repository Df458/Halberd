#ifndef TEXTURE_UTIL_H
#define TEXTURE_UTIL_H
#include <GL/glew.h>
#include <GL/gl.h>
#include <inttypes.h>

//-----------------------------------------------------------------------------
// Struct Section
//-----------------------------------------------------------------------------

typedef struct texture
{
    GLuint texture_id;
    uint16_t texture_width;
    uint16_t texture_height;
}
texture;

typedef struct sprite
{
    texture texture_data;
    int16_t origin_x;
    int16_t origin_y;
}
sprite;

typedef struct font
{
    texture texture_data;
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
    uint8_t loop;
    uint8_t play;
    float offset_x; // offset is for the position in the texture atlas
    float offset_y;
    float size_x; // size is for the size in the texture atlas
    float size_y;
    char* handle;
};

typedef struct spriteset
{
    texture atlas;
    struct animation* animations;
    uint8_t animation_count;

    uint16_t _refcount;
}
spriteset;

typedef struct ui_box
{
    texture texture_data;
    GLuint uv_buffer;

    uint16_t w;
    uint16_t h;

    uint16_t border_w;
    uint16_t border_h;
}
ui_box;

// FIXME: Tileset resource info is never freed
typedef struct tileset // TODO: Fit all tilesets into a texture array(up to 256 simultaneous layers, 32 to be safe!) for extra speed and fewer texture changes
{
    uint8_t layer;
    uint8_t solid[1024];

    char* resource_location;
    char* resource_name;
}
tileset;

//-----------------------------------------------------------------------------
// Loading section
//-----------------------------------------------------------------------------

/*!
 * This loads a png file to a texture.
 * See get_extended_resource_path(io_util.h) for usage details
 */
texture load_resource_to_texture(const char* resource_location, const char* resource_name);

/*!
 * This loads a png file as a font. The width/height for the glyphs is
 * expected to be 1/16th of the texture width/height.
 * See get_extended_resource_path(io_util.h) for usage details
 */
font load_resource_to_font(const char* resource_location, const char* resource_name);

/*!
 * This loads a png file as a sprite. The sprite origin is set to 0x0, the
 * upper-left corner.
 * See get_extended_resource_path(io_util.h) for usage details
 */
sprite load_resource_to_sprite(const char* resource_location, const char* resource_name);

/*!
 * This loads a spriteset file, and constructs a texture atlas from the
 * necessary spritesheets.
 * See get_extended_resource_path(io_util.h) for usage details
 */
spriteset* load_resource_to_spriteset(const char* resource_location, const char* resource_name);

/*!
 * This loads a tileset from a png file.
 * See get_extended_resource_path(io_util.h) for usage details
 */
tileset load_resource_to_tileset(const char* resource_location, const char* resource_name, GLuint texture_handle, uint8_t layer);

//-----------------------------------------------------------------------------
// Freeing section
//-----------------------------------------------------------------------------

/*!
 * This frees an existing spriteset, and all contained resources.
 */
void free_spriteset(spriteset* set);

#endif
