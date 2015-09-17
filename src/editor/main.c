#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "autotile.h"
#include "color.h"
#include "io.h"
#include "render.h"
#include "render_util.h"
#include "settings.h"
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

void closeCallback(GLFWwindow* win)
{
    quit = 1;
}

void sizeCallback(GLFWwindow* win, int width, int height)
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

void cursorMotionCallback(GLFWwindow* window, double pos_x, double pos_y)
{
    if(cursor_middle_button) {
        map_x -= pos_x - cursor_x;
        map_y -= pos_y - cursor_y;
    }
    cursor_x = pos_x;
    cursor_y = pos_y;
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

void cursorScrollCallback(GLFWwindow* window, double delta_x, double delta_y)
{
    if(control_button) {
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

int main(int argc, char** argv)
{
    if(!glfwInit()) {
        error("Failed to intialize GLFW.");
        return 1;
    }

    GLFWwindow* win;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    win = glfwCreateWindow(800, 600, "Halberd Editor", NULL, NULL);
    if(!win) {
        error("Unable to create window.");
        return 1;
    }
    glfwSetWindowCloseCallback(win, closeCallback);
    glfwSetFramebufferSizeCallback(win, sizeCallback);

    glfwSetKeyCallback        (win, keyCodeCallback);
    glfwSetMouseButtonCallback(win, cursorButtonCallback);
    glfwSetCursorPosCallback  (win, cursorMotionCallback);
    glfwSetCursorEnterCallback(win, cursorEnterCallback);
    glfwSetScrollCallback     (win, cursorScrollCallback);

    glfwMakeContextCurrent(win);

	glewExperimental = 1;
	if(glewInit() != GLEW_OK) {
        error("glewInit() failed.");
        glfwDestroyWindow(win);
        return 1;
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
    init_settings();

    init_automaps();
    load_maps("test");
    load_automaps("test");
    color col = {1,1,1,1};
    uint8_t set_id = get_tileset_id("Plains.png");
    uint8_t selected_map = 0;
    int32_t selected_tile_x = 0;
    int32_t selected_tile_y = 0;

    do {
        glfwPollEvents();

        update_view(map_x, map_y, map_zoom);
        update_cursor_data(&selected_map, &selected_tile_x, &selected_tile_y);
        uint8_t can_place = cursor_in_bounds(selected_tile_x, selected_tile_y);

        if(can_place) {
            if(cursor_left_button) {
                /*update_tile(cursor_x / 32, cursor_y / 32, 0, current_id + 1);*/
                place_autotile(selected_map % 3, selected_map / 3, selected_tile_x, selected_tile_y, 0, 1);
            } else if(cursor_right_button) {
                update_tile(selected_tile_x, selected_tile_y, 0, 0, selected_map);
                place_autotile(selected_map % 3, selected_map / 3, selected_tile_x, selected_tile_y, 0, 0);
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 t = ident;
        mat4 cursor_pos = ident;
        uint16_t tile_w = (TILE_WIDTH * map_zoom);
        uint16_t tile_h = (TILE_HEIGHT * map_zoom);
        if(tile_w == 0)
            tile_w = 1;
        if(tile_h == 0)
            tile_h = 1;
        translate(&cursor_pos, selected_tile_x * TILE_WIDTH, selected_tile_y * TILE_HEIGHT, 0);
        draw_maps(t);
        if(can_place) {
            draw_single_tile(set_id, current_id + 1, cursor_pos);
        }
        draw_grid(t);

        glfwSwapBuffers(win);
    } while(!quit);

    destroy_maps();
    destroy_graphics();
    cleanup_settings();

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
