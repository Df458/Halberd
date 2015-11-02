#include "game_input.h"
#include "stdio.h"

// TODO: Allow multiple keys per action
// TODO: Allow action modifiers

int up_key = GLFW_KEY_UP;
int down_key = GLFW_KEY_DOWN;
int left_key = GLFW_KEY_LEFT;
int right_key = GLFW_KEY_RIGHT;
int action_key = GLFW_KEY_Z;
int confirm_key = GLFW_KEY_Z;
int cancel_key = GLFW_KEY_X;
int menu_key = GLFW_KEY_C;
int quit_key = GLFW_KEY_ESCAPE;

uint8_t up_state = 0;
uint8_t down_state = 0;
uint8_t left_state = 0;
uint8_t right_state = 0;
uint8_t action_state = 0;
uint8_t confirm_state = 0;
uint8_t cancel_state = 0;
uint8_t menu_state = 0;
uint8_t quit_state = 0;

uint8_t y_input_prio = 0;

void keyCodeCallback(GLFWwindow* window, int key_code, int scancode, int action, int mod)
{
    // TODO: Better state management
    if(key_code == up_key) {
        up_state = action;
        y_input_prio = 1;
    } else if(key_code == down_key) {
        down_state = action;
        y_input_prio = 1;
    } else if(key_code == left_key) {
        left_state = action;
        y_input_prio = 0;
    } else if(key_code == right_key) {
        right_state = action;
        y_input_prio = 0;
    } else if(key_code == action_key && action == GLFW_PRESS)
        action_state = action;
    else if(key_code == confirm_key && action == GLFW_PRESS)
        confirm_state = action;
    else if(key_code == cancel_key && action == GLFW_PRESS)
        cancel_state = action;
    else if(key_code == menu_key)
        menu_state = action;
    else if(key_code == quit_key)
        quit_state = action;
}

void closeCallback(GLFWwindow* win)
{
    quit_state = 1;
}

void init_input(GLFWwindow* win)
{
    glfwSetKeyCallback(win, keyCodeCallback);
    glfwSetWindowCloseCallback(win, closeCallback);
}

void update_input()
{
    action_state = 0;
    confirm_state = 0;
    cancel_state = 0;
}

uint8_t get_directional_input()
{
    int8_t x = 0;
    int8_t y = 0;

    if(left_state > 0)
        --x;
    if(right_state > 0)
        ++x;
    if(up_state > 0)
        --y;
    if(down_state > 0)
        ++y;

    if (x && y) {
        if(y_input_prio)
            x = 0;
        else
            y = 0;
    }
    if(x) {
        return x > 0 ? 2 : 1;
    } else if(y) {
        return y > 0 ? 4 : 3;
    }

    return 0;
}

uint8_t get_action_input()
{
    return action_state;
}

uint8_t get_confirm_input()
{
    return confirm_state;
}

uint8_t get_cancel_input()
{
    return cancel_state;
}

uint8_t get_menu_input()
{
    return menu_state;
}

uint8_t get_quit_input()
{
    return quit_state;
}

void set_directional_input(int key_up, int key_down, int key_left, int key_right)
{
    up_key = key_up;
    down_key = key_down;
    left_key = key_left;
    right_key = key_right;
}

void set_action_input(int key)
{
    action_key = key;
}

void set_confirm_input(int key)
{
    confirm_key = key;
}

void set_cancel_input(int key)
{
    cancel_key = key;
}

void set_menu_input(int key)
{
    menu_key = key;
}

void set_quit_input(int key)
{
    quit_key = key;
}
