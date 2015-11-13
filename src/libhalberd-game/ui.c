#include "render_util.h"
#include "io.h"
#include "ui.h"
#include "controls.h"
#include <stdlib.h>
#include <string.h>

message_link* current_link = 0;
message_link* final_link = 0;
font* default_font = 0;

uint8_t init_ui()
{
    if(!(default_font = load_font("Testfont.png")));
        return 0;
}

void update_ui(float dt)
{
    if(!current_link)
        return;

    if(current_link->pen >= current_link->len && (get_confirm_input() == 1 || get_cancel_input() == 1 || get_action_input() == 1)) {
        message_link* prev = current_link;
        current_link = current_link->next_link;
        free(prev->message);
        free(prev);
    } else {
        if(current_link->pen < current_link->len)
            current_link->pen++;
        if(get_cancel_input() == 1)
            current_link->pen = current_link->len;
    }
}

void draw_ui()
{
    if(!current_link)
        return;

    if(current_link->pen > 0)
        draw_textbox(default_font, current_link->message, 0, 0, 150, 250, 48, current_link->pen);
}

void add_message(const char* message)
{
    if(!current_link) {
        current_link = malloc(sizeof(message_link));
        final_link = current_link;
    } else {
        final_link->next_link = malloc(sizeof(message_link));
        final_link = final_link->next_link;
    }
    uint16_t max_len = 250 / default_font->w;
    uint16_t max_lines = 48 / default_font->h;
    message_link* link = final_link;
    link->next_link = 0;
    link->len = strlen(message);
    char* token_str = calloc(link->len + 1, sizeof(char));
    strcpy(token_str, message);
    
    uint16_t lines = 0;
    uint16_t pen = 0;
    char* s = strtok(token_str, "\n");
    while(s) {
        ++lines;
        if(lines > 1) {
            link->message = realloc(link->message, pen + strlen(s) + 1);
            link->message[pen] = '\n';
            ++pen;
        } else {
            link->message = calloc(strlen(s) + 1, sizeof(char));
        }
        uint16_t s_len = strlen(s);
        strncpy(link->message + pen, s, s_len);
        if(s_len > max_len) {
            uint8_t too_long = 1;
            uint8_t sub_pen = pen;
            do {
                int16_t i;
                uint8_t success = 0;
                for(i = sub_pen + max_len; i >= sub_pen && link->message[i] != '\n'; --i) {
                    if(link->message[i] == ' ') {
                        link->message[i] = '\n';
                        sub_pen = i + 1;
                        ++lines;
                        success = 1;
                        break;
                    }
                }
                if(i < sub_pen && sub_pen > 0 && !success) {
                    link->message = realloc(link->message, sub_pen + s_len + 2);
                    link->message[sub_pen + s_len + 1] = link->message[sub_pen + s_len];
                    for(char* c = link->message + sub_pen + s_len + 1; c != link->message + sub_pen + max_len; --c)
                        c[0] = c[-1];
                    link->message[sub_pen + max_len] = '\n';
                    ++lines;
                    ++pen;
                    sub_pen += max_len + 1;
                }
            } while(sub_pen - pen < s_len - max_len);
        }
        pen += s_len;
        s = strtok(0, "\n");
    }

    while(lines > max_lines) {
        lines -= max_lines;
        uint8_t nco = 0;
        int c;
        for(c = 0; nco < max_lines && link->message[c]; ++c) {
            if(link->message[c] == '\n')
                ++nco;
        }
        add_message(link->message + c);
        link->message = realloc(link->message, c - 1);
        pen = c - 1;
    }
    link->message = realloc(link->message, pen + 1);
    link->message[pen] = 0;
    link->len = pen;
    link->pen = 0;

    free(token_str);
}

uint8_t ui_present()
{
    return current_link != 0;
}

void destroy_ui()
{
    for(message_link* i = current_link; i;) {
        message_link* m = i;
        i = i->next_link;
        free(m->message);
        free(m);
    }

    if(default_font) {
        glDeleteTextures(1, &default_font->texture);
        free(default_font);
    }
}
