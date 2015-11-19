#ifndef UI_H
#define UI_H
#include <inttypes.h>
#include <stdbool.h>

bool init_ui();
void update_ui(float dt);
void draw_ui();
void add_message(const char* message);
uint8_t ui_present();
void destroy_ui();

typedef struct message_link
{
    char* message;
    struct message_link* next_link;
    uint16_t pen;
    uint16_t len;
} message_link;

#endif
