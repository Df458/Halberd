#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "autotile.h"
#include "color.h"
#include "io.h"
#include "render.h"
#include "render_util.h"
#include "settings.h"
#include "stack.h"
#include "util.h"

uint8_t quit = 0;
uint32_t current_id = 0;
int32_t cursor_x = 0;
int32_t cursor_y = 0;
int16_t map_x = 0;
int16_t map_y = 0;
float map_zoom = 1;
uint8_t cursor_left_button = 0;
uint8_t cursor_right_button = 0;
uint8_t cursor_middle_button = 0;
uint8_t control_button = 0;

uint8_t selected_map = 0;
int32_t selected_tile_x = 0;
int32_t selected_tile_y = 0;
uint8_t set_id = 0;

void floodFill()
{
    int32_t p_x = selected_tile_x;
    int32_t p_y = selected_tile_y;
    uint8_t target = get_autotile(selected_map % 3, selected_map / 3, p_x, p_y, 0);
    if(target == 1)
        return;
    stack* cursor_stack = stack_make();
    int16_t* first_pair = calloc(sizeof(int16_t), 4);
    first_pair[0] = p_x;
    first_pair[1] = p_y;
    first_pair[2] = selected_map % 3;
    first_pair[3] = selected_map / 3;
    stack_push(&cursor_stack, first_pair);
    uint16_t i = 0;
    uint16_t j = 1;
    do {
        ++i;
        int16_t* current_pair = stack_pop(&cursor_stack, 0);
        nulltest(current_pair);
        uint16_t pxm = current_pair[2];
        uint16_t pym = current_pair[3];
        get_true_tile_position(&pxm, &pym, &current_pair[0], &current_pair[1]);
        place_autotile(pxm, pym, current_pair[0], current_pair[1], 0, 1);
        for(uint8_t i = 1; i < 7; i++) {
            if(i == 2 || i == 5)
                i++;
            uint8_t a = get_autotile(pxm, pym, current_pair[0] + surround_x_table[i], current_pair[1] + surround_y_table[i], 0);
            if(a != target)
                continue;
            ++j;
            int16_t* cursor_pair = calloc(sizeof(int16_t), 4);
            cursor_pair[0] = current_pair[0] + surround_x_table[i];
            cursor_pair[1] = current_pair[1] + surround_y_table[i];
            cursor_pair[2] = pxm;
            cursor_pair[3] = pym;
            stack_push(&cursor_stack, cursor_pair);
        }
        --j;
        free(current_pair);
    } while(!stack_empty(cursor_stack));
}

void sizeCallback(GtkGLArea* area, gint width, gint height, gpointer user_data)
{
    glViewport(0, 0, width, height);
    update_camera(width, height);
}

void keyCodeCallback(GLFWwindow* window, int key_code, int scancode, int action, int mod)
{
    // TODO: Add modifier support, press/release callbacks vs down
    if(action == GLFW_PRESS || action == GLFW_RELEASE) {
        if(key_code == GLFW_KEY_S && mod == GLFW_MOD_CONTROL && action == GLFW_PRESS) {
            save_maps();
            save_automaps();
        } else if(key_code == GLFW_KEY_N && mod == GLFW_MOD_CONTROL && action == GLFW_PRESS) {
            clear_maps();
            clear_automaps();
        } else if(key_code == GLFW_KEY_LEFT_CONTROL) {
            if(action == GLFW_PRESS)
                control_button = 1;
            else if(action == GLFW_RELEASE)
                control_button = 0;
        }
    }
}

void cursorEnterCallback(GLFWwindow* window, int entered)
{
}

gboolean cursorMotionCallback(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
    double pos_x = event->motion.x;
    double pos_y = event->motion.y;
    if(cursor_middle_button) {
        map_x -= pos_x - cursor_x;
        map_y -= pos_y - cursor_y;
    }
    cursor_x = pos_x;
    cursor_y = pos_y;
    if(cursor_left_button) {
        /*update_tile(cursor_x / 32, cursor_y / 32, 0, current_id + 1);*/
        place_autotile(selected_map % 3, selected_map / 3, selected_tile_x, selected_tile_y, 0, 1);
    } else if(cursor_right_button) {
        update_tile(selected_tile_x, selected_tile_y, 0, 0, selected_map);
        place_autotile(selected_map % 3, selected_map / 3, selected_tile_x, selected_tile_y, 0, 0);
    }
    gtk_widget_queue_draw(widget);
    return FALSE;
}

gboolean cursorPressCallback(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
    uint8_t button = event->button.button;
    if(button == 1) {
        cursor_left_button = 1;
        /*update_tile(cursor_x / 32, cursor_y / 32, 0, current_id + 1);*/
        place_autotile(selected_map % 3, selected_map / 3, selected_tile_x, selected_tile_y, 0, 1);
    } else if(button == 2) {
        cursor_middle_button = 1;
        floodFill();
    } else if(button == 3) {
        cursor_right_button = 1;
        update_tile(selected_tile_x, selected_tile_y, 0, 0, selected_map);
        place_autotile(selected_map % 3, selected_map / 3, selected_tile_x, selected_tile_y, 0, 0);
    }
    gtk_widget_queue_draw(widget);
    return FALSE;
}

gboolean cursorReleaseCallback(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
    uint8_t button = event->button.button;
    if(button == 1)
        cursor_left_button = 0;
    else if(button == 2)
        cursor_middle_button = 0;
    else if(button == 3)
        cursor_right_button = 0;
    return FALSE;
}

gboolean cursorScrollCallback(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
    /*double delta_x = 0;*/
    double delta_y = 0;
    if(event->scroll.direction == GDK_SCROLL_SMOOTH) {
        /*delta_x = event->scroll.delta_x;*/
        delta_y = event->scroll.delta_y;
    } else if(event->scroll.direction == GDK_SCROLL_UP) {
        delta_y = 1;
    } else if(event->scroll.direction == GDK_SCROLL_DOWN) {
        delta_y = -1;
    }
    if(event->scroll.state & GDK_CONTROL_MASK) {
        map_zoom += delta_y / 10.0f;
        if(map_zoom > 5)
            map_zoom = 5;
        else if(map_zoom < 0.01)
            map_zoom = 0.01;
    } else {
        current_id += delta_y;
        if(current_id < 0)
            current_id = 1023;
        else if(current_id >= 1024)
            current_id = 0;
    }

    gtk_widget_queue_draw(widget);
    return FALSE;
}

void cursorButtonCallback(GLFWwindow* window, int button, int action, int mod)
{
    // TODO: Add modifier support, press/release callbacks vs down
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        if(action == GLFW_PRESS)
            cursor_left_button = 1;
        else if(action == GLFW_RELEASE)
            cursor_left_button = 0;
    } else if(button == GLFW_MOUSE_BUTTON_RIGHT) {
        if(action == GLFW_PRESS)
            cursor_right_button = 1;
        else if(action == GLFW_RELEASE)
            cursor_right_button = 0;
    } else if(button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if(action == GLFW_PRESS)
            cursor_middle_button = 1;
        else if(action == GLFW_RELEASE)
            cursor_middle_button = 0;
    }
}

uint8_t cursor_in_bounds(int32_t true_x, int32_t true_y)
{
    return true_x >= 0 && true_y >= 0;
}

void update_cursor_data(uint8_t* map, int32_t* tile_x, int32_t* tile_y)
{
    *tile_x = ((cursor_x + map_x) / (TILE_WIDTH * map_zoom));
    *tile_y = ((cursor_y + map_y) / (TILE_HEIGHT * map_zoom));
    *map = (*tile_x / TILEMAP_DIMS) + (*tile_y / TILEMAP_DIMS * 3);
    *tile_x %= TILEMAP_DIMS;
    *tile_y %= TILEMAP_DIMS;
}

static gboolean render(GtkGLArea* area, GdkGLContext* ctx)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    update_view(map_x, map_y, map_zoom);
    update_cursor_data(&selected_map, &selected_tile_x, &selected_tile_y);
    uint8_t can_place = cursor_in_bounds(selected_tile_x, selected_tile_y);

    mat4 t = ident;
    mat4 cursor_pos = ident;
    uint16_t tile_w = (TILE_WIDTH * map_zoom);
    uint16_t tile_h = (TILE_HEIGHT * map_zoom);
    if(tile_w == 0)
        tile_w = 1;
    if(tile_h == 0)
        tile_h = 1;
    translate(&cursor_pos, selected_tile_x * TILE_WIDTH + (selected_map % 3 * TILEMAP_DIMS * TILE_WIDTH), selected_tile_y * TILE_HEIGHT + (selected_map / 3 * TILEMAP_DIMS * TILE_HEIGHT), 0);
    draw_maps(t);
    if(can_place) {
        draw_single_tile(set_id, current_id + 1, cursor_pos);
    }
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j) {
            translate(&t, TILEMAP_DIMS * TILE_WIDTH * i, TILEMAP_DIMS * TILE_HEIGHT * j, 0);
            draw_grid(t);
        }
    
    glFlush();

    return TRUE;
}

static void gl_init(GtkGLArea* area)
{
    gtk_gl_area_make_current(area);
    if (gtk_gl_area_get_error (area) != NULL)
        return;

    glewExperimental = 1;
    if(glewInit() != GLEW_OK) {
        error("glewInit() failed.");
        // TODO: Exit gracefully
        return;
    }
    glGetError(); // Because GLEW is silly. <http://stackoverflow.com/questions/20034615/why-does-glewinit-result-in-gl-invalid-enum-after-making-some-calls-to-glfwwin>
    GLuint VAO; // FIXME: Stupid Hack. <http://stackoverflow.com/questions/13403807/glvertexattribpointer-raising-gl-invalid-operation>
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

    init_graphics();
    init_grid();

    init_automaps();
    load_maps("test");
    load_automaps("test");
    color col = {1,1,1,1};
    set_id = get_tileset_id("Plains.png");
}

static void gl_destroy(GtkGLArea* area)
{
    destroy_maps();
    destroy_graphics();
    fprintf(stderr, "Exiting gracefully...\n");
}

int main(int argc, char* argv[])
{
    GtkWidget* window;
    GtkWidget* header;

    GtkWidget* button_new;
    GtkWidget* button_open;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_events(window, GDK_ALL_EVENTS_MASK);
    gtk_container_set_border_width (GTK_CONTAINER (window), 3);
    gtk_window_set_default_size (GTK_WINDOW (window), 1024, 768);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    header = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "Halberd RPG Engine");
    gtk_header_bar_set_decoration_layout(GTK_HEADER_BAR(header), "menu:close");
    gtk_window_set_titlebar(GTK_WINDOW(window), header);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); 

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* ctx = gtk_gl_area_new();
    gtk_gl_area_set_has_depth_buffer(GTK_GL_AREA(ctx), TRUE);
    gtk_gl_area_set_auto_render(GTK_GL_AREA(ctx), TRUE);
    gtk_gl_area_set_required_version(GTK_GL_AREA(ctx), 3, 3);

    button_new = gtk_button_new_from_icon_name("document-new-symbolic", GTK_ICON_SIZE_LARGE_TOOLBAR);
    button_open = gtk_button_new_from_icon_name("document-open-symbolic", GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button_new);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button_open);

    g_signal_connect(ctx, "render", G_CALLBACK(render), NULL);
    g_signal_connect(ctx, "resize", G_CALLBACK(sizeCallback), NULL);
    g_signal_connect(ctx, "realize", G_CALLBACK(gl_init), NULL);
    g_signal_connect(ctx, "unrealize", G_CALLBACK(gl_destroy), NULL);
    g_signal_connect(ctx, "motion-notify-event", G_CALLBACK(cursorMotionCallback), NULL);
    g_signal_connect(ctx, "button-press-event", G_CALLBACK(cursorPressCallback), NULL);
    g_signal_connect(ctx, "button-release-event", G_CALLBACK(cursorReleaseCallback), NULL);
    g_signal_connect(ctx, "scroll-event", G_CALLBACK(cursorScrollCallback), NULL);
    gtk_widget_set_events(ctx, gtk_widget_get_events(ctx) | GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_SCROLL_MASK);

    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_box_pack_start(GTK_BOX(box), ctx, TRUE, TRUE, 0);

    gtk_widget_show_all(window);

    init_settings();

    gtk_main();

    cleanup_settings();

    return 0;
}
