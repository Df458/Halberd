#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "color.h"
#include "controls.h"
#include "game_input.h"
#include "game-render.h"
#include "io.h"
#include "player.h"
#include "render_util.h"
#include "settings.h"
#include "ui.h"
#include "util.h"

void sizeCallback(GLFWwindow* win, int width, int height)
{
    glViewport(0, 0, width, height);
    update_camera(width, height);
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

    win = glfwCreateWindow(800, 600, "Halberd", NULL, NULL);
    if(!win) {
        error("Unable to create window.");
        return 1;
    }
    glfwSetFramebufferSizeCallback(win, sizeCallback);
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

    init_graphics();
    update_camera(800, 600);
    init_input(win);
    init_player(0, 0);
    init_ui();
    init_settings();

    /*init_maps();*/
    // TODO: Figure out the correct way to start up
    /*get_tileset_id("Plains.png");*/
    /*load_maps("test");*/

    float delta = 0;
    glfwSetTime(0);
    do {
        delta = glfwGetTime();
        glfwSetTime(0);
        glfwPollEvents();

        if(!ui_present())
            update_player(delta);
        update_actors(delta);
        update_ui(delta);
        update_input_states();

        render_game();

        glfwSwapBuffers(win);
    } while(!get_quit_input());

    destroy_player();
    destroy_actors();
    destroy_maps();
    destroy_ui();
    destroy_graphics();
    cleanup_settings();

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
