#include "cursor.h"
#include "editor-render.h"
#include "render_util.h"
#include "util.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

GLuint grid_program = 0;
GLuint grid_position_attrib = 0;
GLuint grid_vertex_attrib = 0;
GLuint grid_horizontal_attrib = 0;
GLuint grid_transform_uniform = 0;
GLuint grid_color_uniform = 0;
GLuint grid_texture_uniform = 0;

GLuint grid_dash_texture = 0;
GLuint grid_buffer = 0;
GLuint grid_horiz_buffer = 0;
GLuint grid_position_buffer = 0;

bool init_render()
{
    glewExperimental = 1;
    if(glewInit() != GLEW_OK) {
        error("glewInit() failed.");
        return false;
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
    if(!init_grid())
        return false;

    /*color col = {1,1,1,1};*/
    // TODO: Move this somewhere
    /*set_id = get_tileset_id("Plains.png");*/
    return true;
}

bool render_editor()
{
    cursor c = cursor_get();
    map_view v = view_get();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    update_view(v.x, v.y, v.zoom);
    /*update_cursor_data(&selected_map, &selected_tile_x, &selected_tile_y);*/
    bool can_place = cursor_in_bounds(c.selected_tile_x, c.selected_tile_y);

    mat4 t = ident;
    mat4 cursor_pos = ident;
    uint16_t tile_w = (TILE_WIDTH * v.zoom);
    uint16_t tile_h = (TILE_HEIGHT * v.zoom);
    if(tile_w == 0)
        tile_w = 1;
    if(tile_h == 0)
        tile_h = 1;
    translate(&cursor_pos, c.selected_tile_x * TILE_WIDTH + (c.selected_map % 3 * TILEMAP_DIMS * TILE_WIDTH), c.selected_tile_y * TILE_HEIGHT + (c.selected_map / 3 * TILEMAP_DIMS * TILE_HEIGHT), 0);
    draw_maps(t);
    if(can_place) {
        draw_single_tile(c.set_id, c.current_id, cursor_pos);
    }
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j) {
            translate(&t, TILEMAP_DIMS * TILE_WIDTH * i, TILEMAP_DIMS * TILE_HEIGHT * j, 0);
            draw_grid(t);
        }
    
    glFlush();

    return true;
}

void destroy_render()
{
    // TODO: Re-enable these
    /*destroy_maps();*/
    destroy_graphics();
    fprintf(stderr, "Exiting gracefully...\n");
}

bool init_grid()
{
    create_program(&grid_program, GRID_VERTEX_SHADER, GRID_FRAGMENT_SHADER);
    grid_position_attrib = glGetAttribLocation(grid_program, "position");
    grid_horizontal_attrib = glGetAttribLocation(grid_program, "horizontal");
    grid_vertex_attrib = glGetAttribLocation(grid_program, "vertex_pos");
    grid_transform_uniform = glGetUniformLocation(grid_program, "transform");
    grid_texture_uniform = glGetUniformLocation(grid_program, "texture");
    grid_color_uniform = glGetUniformLocation(grid_program, "color");
    if(checkGLError())
        return false;

    glGenBuffers(1, &grid_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, grid_buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), GRID_BUFFER_DATA, GL_STATIC_DRAW);
    if(checkGLError())
        return false;

    float position_buffer_data[TILEMAP_DIMS * 6];
    GLuint horiz_buffer_data[TILEMAP_DIMS * 2];

    for(int i = 0; i < TILEMAP_DIMS; ++i) {
        position_buffer_data[i*3] = i * 32;
        position_buffer_data[i*3 + 1] = 0;
        position_buffer_data[i*3 + 2] = -1;
        horiz_buffer_data[i] = 0;
    }

    for(int i = 0; i < TILEMAP_DIMS; ++i) {
        position_buffer_data[(TILEMAP_DIMS * 3) + (i*3)] = 0;
        position_buffer_data[(TILEMAP_DIMS * 3) + (i*3 + 1)] = i * 32;
        position_buffer_data[(TILEMAP_DIMS * 3) + (i*3 + 2)] = -1;
        horiz_buffer_data[(TILEMAP_DIMS) + i] = 1;
    }

    glGenBuffers(1, &grid_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, grid_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * TILEMAP_DIMS * sizeof(float), position_buffer_data, GL_STATIC_DRAW);
    if(checkGLError())
        return false;


    glGenBuffers(1, &grid_horiz_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, grid_horiz_buffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * TILEMAP_DIMS * sizeof(GLuint), horiz_buffer_data, GL_STATIC_DRAW);
    if(checkGLError())
        return false;

    glGenTextures(1, &grid_dash_texture);
    glBindTexture(GL_TEXTURE_2D, grid_dash_texture);
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 5, 0, GL_RGBA, GL_UNSIGNED_BYTE, GRID_TEXTURE_BUFFER_DATA);
    if(checkGLError())
        return false;

    return true;
}

void draw_grid(mat4 transform)
{
    glUseProgram(grid_program);

    glEnableVertexAttribArray(grid_vertex_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, grid_buffer);
    glVertexAttribPointer(grid_vertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    checkGLError();

    glEnableVertexAttribArray(grid_position_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, grid_position_buffer);
    glVertexAttribPointer(grid_position_attrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(grid_horizontal_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, grid_horiz_buffer);
    glVertexAttribIPointer(grid_horizontal_attrib, 1, GL_UNSIGNED_INT, 0, (void*)0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grid_dash_texture);
    glUniform1i(grid_texture_uniform, 0);
    checkGLError();

    mat4 tt = ident;
    translate(&tt, 1, 1, -10);
    mat4 camera = get_camera();
    mat4 view = get_view();
    mat4 final = mul(mul(camera, view), mul(transform, tt));
    glUniformMatrix4fv(grid_transform_uniform, 1, GL_FALSE, final.data);

    glVertexAttribDivisor(grid_vertex_attrib, 0);
    glVertexAttribDivisor(grid_position_attrib, 1);
    glVertexAttribDivisor(grid_horizontal_attrib, 1);
    checkGLError();

    glDrawArraysInstanced(GL_LINES, 0, 2, TILEMAP_DIMS * 2);

    glDisableVertexAttribArray(grid_vertex_attrib);
    glVertexAttribDivisor(grid_position_attrib, 0);
    glVertexAttribDivisor(grid_horizontal_attrib, 0);

}

void size_callback(int width, int height)
{
    glViewport(0, 0, width, height);
    update_camera(width, height);
}
