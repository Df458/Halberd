#include <dfgame-game.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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

void keyCodeCallback(GLFWwindow* window, int key_code, int scancode, int action, int mod)
{
    if(action == GLFW_REPEAT)
        return;
    // TODO: Better state management
    if(key_code == up_key) {
        set_up_state(action == GLFW_PRESS);
    } else if(key_code == down_key) {
        set_down_state(action == GLFW_PRESS);
    } else if(key_code == left_key) {
        set_left_state(action == GLFW_PRESS);
    } else if(key_code == right_key) {
        set_right_state(action == GLFW_PRESS);
    } else if(key_code == action_key && action == GLFW_PRESS)
        set_action_state(true);
    else if(key_code == confirm_key && action == GLFW_PRESS)
        set_confirm_state(true);
    else if(key_code == cancel_key && action == GLFW_PRESS)
        set_cancel_state(true);
    else if(key_code == menu_key)
        set_menu_state(action == GLFW_PRESS);
    else if(key_code == quit_key)
        set_quit_state(action == GLFW_PRESS);
}

void closeCallback(GLFWwindow* win)
{
    set_quit_state(true);
}

void init_input(GLFWwindow* win)
{
    glfwSetKeyCallback(win, keyCodeCallback);
    glfwSetWindowCloseCallback(win, closeCallback);
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
