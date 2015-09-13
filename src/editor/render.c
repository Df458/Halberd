#include "render.h"
#include "render_util.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

uint8_t init_grid()
{
    create_program(&grid_program, GRID_VERTEX_SHADER, GRID_FRAGMENT_SHADER);
    grid_position_attrib = glGetAttribLocation(grid_program, "position");
    grid_horizontal_attrib = glGetAttribLocation(grid_program, "horizontal");
    grid_vertex_attrib = glGetAttribLocation(grid_program, "vertex_pos");
    grid_transform_uniform = glGetUniformLocation(grid_program, "transform");
    grid_texture_uniform = glGetUniformLocation(grid_program, "texture");
    grid_color_uniform = glGetUniformLocation(grid_program, "color");
    if(checkGLError())
        return 0;

    glGenBuffers(1, &grid_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, grid_buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), GRID_BUFFER_DATA, GL_STATIC_DRAW);
    if(checkGLError())
        return 0;

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
        return 0;


    glGenBuffers(1, &grid_horiz_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, grid_horiz_buffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * TILEMAP_DIMS * sizeof(GLuint), horiz_buffer_data, GL_STATIC_DRAW);
    if(checkGLError())
        return 0;

    glGenTextures(1, &grid_dash_texture);
    glBindTexture(GL_TEXTURE_2D, grid_dash_texture);
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 5, 0, GL_RGBA, GL_UNSIGNED_BYTE, GRID_TEXTURE_BUFFER_DATA);
    if(checkGLError())
        return 0;

    return 1;
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
