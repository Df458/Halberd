#include <inttypes.h>
#include <stdbool.h>

typedef struct cursor {
    int32_t screen_x;
    int32_t screen_y;
    int32_t selected_tile_x;
    int32_t selected_tile_y;

    uint8_t left_button;
    uint8_t right_button;
    uint8_t middle_button;
    uint8_t control_button;

    uint8_t selected_map;
    uint8_t set_id;
    uint32_t current_id;
} cursor;

typedef struct map_view {
    int32_t x;
    int32_t y;
    float zoom;
} map_view;

void cursor_init();
void cursor_set_position(int32_t x, int32_t y);
void cursor_set_id(uint32_t id);
void map_set_position(int32_t x, int32_t y);
void map_drag(int32_t x, int32_t y);
cursor cursor_get();
map_view view_get();
bool cursor_in_bounds(int32_t true_x, int32_t true_y);
void cursor_place_tile();
void cursor_place_tile_id(uint8_t tile);
void cursor_place_tile_at(uint8_t tile, int32_t x, int32_t y);
void cursor_place_line(int32_t x, int32_t y);
void cursor_place_line_id(uint8_t tile, int32_t x, int32_t y);
void cursor_flood_fill(uint8_t tile);
void cursor_zoom(double input);
