#ifndef INPUT_H
#define INPUT_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <inttypes.h>

void init_input(GLFWwindow* win);
void update_input();

uint8_t get_directional_input();
uint8_t get_action_input();
uint8_t get_confirm_input();
uint8_t get_cancel_input();
uint8_t get_menu_input();
uint8_t get_quit_input();

void set_directional_input(int key_up, int key_down, int key_left, int key_right);
void set_action_input(int key);
void set_confirm_input(int key);
void set_cancel_input(int key);
void set_menu_input(int key);
void set_quit_input(int key);

#endif
