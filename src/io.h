#include "actor.h"
#include "map.h"
#include "render_util.h"

unsigned char* load_file(const char* filepath);
GLuint load_image(const char* path, uint16_t* w, uint16_t* h);
sprite make_sprite(const char* path);
font* load_font(const char* path);
spriteset* get_spriteset(const char* path);
void destroy_spriteset(spriteset* set);
uint8_t load_tileset(const char* path, tileset** tiles, uint8_t count, GLuint texture_handle);
uint8_t load_map(const char* path, tilemap* map);
void save_map(const char* path, tilemap* map);
int16_t load_boxes(const char* path, ui_box** boxes);
