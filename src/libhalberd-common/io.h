#include "actor.h"
#include "map.h"
#include "render_util.h"
#include "quadtree.h"

uint8_t load_map(const char* path, tilemap* map);
void save_map(const char* path, tilemap* map);
int16_t load_boxes(const char* path, ui_box** boxes);
