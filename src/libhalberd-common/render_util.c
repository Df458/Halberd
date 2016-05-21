#include <dfgame-common.h>
#include "render_util.h"
#include "map.h"
#include "io.h"
#include "texture_util.h"
#include "settings.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

program sprite_program;
program tile_program;
program font_program;
program single_tile_program;
program box_program;
GLuint sprite_texture_uniform = 0;
GLuint sprite_vertex_attrib = 0;
GLuint sprite_color_uniform = 0;
GLuint sprite_uv_dims_uniform = 0;
GLuint sprite_uv_offs_uniform = 0;
GLuint sprite_transform_uniform = 0;

GLuint tile_vertex_attrib = 0;
GLuint tile_texture_uniform = 0;
GLuint tile_transform_uniform = 0;
GLuint tile_position_attrib = 0;
GLuint tile_id_attrib = 0;
GLuint tile_tid_attrib = 0;
GLuint tile_color_uniform = 0;

GLuint single_tile_vertex_attrib = 0;
GLuint single_tile_texture_uniform = 0;
GLuint single_tile_transform_uniform = 0;
GLuint single_tile_id_uniform = 0;
GLuint single_tile_tid_uniform = 0;
GLuint single_tile_color_uniform = 0;

GLuint font_vertex_attrib = 0;
GLuint font_texture_uniform = 0;
GLuint font_transform_uniform = 0;
GLuint font_position_attrib = 0;
GLuint font_id_attrib = 0;
GLuint font_color_uniform = 0;

GLuint box_vertex_attrib = 0;
GLuint box_uv_attrib = 0;
GLuint box_outer_attrib = 0;
GLuint box_texture_uniform = 0;
GLuint box_scale_inner_uniform = 0;
GLuint box_scale_full_uniform = 0;
GLuint box_transform_uniform = 0;

GLuint box_buffer = 0;
GLuint box_s_buffer = 0;
GLuint blank_texture = 0;
GLuint tile_position_buffer = 0;
GLuint font_position_buffer = 0;
GLuint font_buffer = 0;

camera* viewport_camera = 0;
uint16_t view_width = 0;
uint16_t view_height = 0;

ui_box* boxes = 0;
int16_t loaded_boxes;

uint8_t init_graphics(void)
{
    if(!init_renderer())
        return 0;

    glClearColor(.6, .6, .6, 1);

    sprite_program      = create_program(SPRITE_VERTEX_SHADER,       SPRITE_FRAGMENT_SHADER);
    tile_program        = create_program(TILE_VERTEX_SHADER,         TILE_FRAGMENT_SHADER);
    font_program        = create_program(FONT_VERTEX_SHADER,         FONT_FRAGMENT_SHADER);
    single_tile_program = create_program(SINGLE_TILE_VERTEX_SHADER,  TILE_FRAGMENT_SHADER);
    box_program         = create_program(BOX_VERTEX_SHADER,          BOX_FRAGMENT_SHADER);

    /* sprite_color_uniform = glGetUniformLocation(sprite_program, "color"); */
    /* sprite_uv_offs_uniform = glGetUniformLocation(sprite_program, "uv_offset"); */
    /* sprite_uv_dims_uniform = glGetUniformLocation(sprite_program, "uv_dimensions"); */
    /* sprite_texture_uniform = glGetUniformLocation(sprite_program, "texture"); */
    /* sprite_transform_uniform = glGetUniformLocation(sprite_program, "transform"); */
    /* sprite_vertex_attrib = glGetAttribLocation(sprite_program, "vertex_pos"); */
    /* if(checkGLError()) */
    /*     return 0; */

    /* tile_transform_uniform = glGetUniformLocation(tile_program, "transform"); */
    /* tile_texture_uniform = glGetUniformLocation(tile_program, "texture"); */
    /* tile_vertex_attrib = glGetAttribLocation(tile_program, "vertex_pos"); */
    /* tile_position_attrib = glGetAttribLocation(tile_program, "position"); */
    /* tile_id_attrib = glGetAttribLocation(tile_program, "id_in"); */
    /* tile_tid_attrib = glGetAttribLocation(tile_program, "tid_in"); */
    /* tile_color_uniform = glGetUniformLocation(tile_program, "color"); */
    /* if(checkGLError()) */
    /*     return 0; */

    /* font_transform_uniform = glGetUniformLocation(tile_program, "transform"); */
    /* font_texture_uniform = glGetUniformLocation(tile_program, "texture"); */
    /* font_vertex_attrib = glGetAttribLocation(tile_program, "vertex_pos"); */
    /* font_position_attrib = glGetAttribLocation(tile_program, "position"); */
    /* font_id_attrib = glGetAttribLocation(tile_program, "id_in"); */
    /* font_color_uniform = glGetUniformLocation(tile_program, "color"); */
    /* if(checkGLError()) */
    /*     return 0; */

    /* single_tile_transform_uniform = glGetUniformLocation(single_tile_program, "transform"); */
    /* single_tile_texture_uniform = glGetUniformLocation(single_tile_program, "texture"); */
    /* single_tile_vertex_attrib = glGetAttribLocation(single_tile_program, "vertex_pos"); */
    /* single_tile_id_uniform = glGetUniformLocation(single_tile_program, "id_in"); */
    /* single_tile_tid_uniform = glGetUniformLocation(single_tile_program, "tid_in"); */
    /* single_tile_color_uniform = glGetUniformLocation(single_tile_program, "color"); */
    /* if(checkGLError()) */
    /*     return 0; */

    /* box_vertex_attrib = glGetAttribLocation(box_program, "vertex_pos"); */
    /* box_uv_attrib = glGetAttribLocation(box_program, "vertex_uv"); */
    /* box_outer_attrib = glGetAttribLocation(box_program, "vertex_rim"); */
    /* box_texture_uniform = glGetUniformLocation(box_program, "texture"); */
    /* box_scale_inner_uniform = glGetUniformLocation(box_program, "scale_inner"); */
    /* box_scale_full_uniform = glGetUniformLocation(box_program, "scale_full"); */
    /* box_transform_uniform = glGetUniformLocation(box_program, "transform"); */
    /* if(checkGLError()) */
    /*     return 0; */

    // TODO: Move this to a separate box loading bit, maybe in game?
    /*loaded_boxes = load_boxes("boxes.xml", &boxes);*/
    /*if(loaded_boxes == -1)*/
        /*return 0;*/

    glGenBuffers(1, &box_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, box_buffer);
	glBufferData(GL_ARRAY_BUFFER, 162 * sizeof(float), BOX_BUFFER_DATA, GL_STATIC_DRAW);
    if(checkGLError())
        return 0;
    glGenBuffers(1, &box_s_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, box_s_buffer);
	glBufferData(GL_ARRAY_BUFFER, 102 * sizeof(float), BOX_S_BUFFER_DATA, GL_STATIC_DRAW);
    if(checkGLError())
        return 0;

    float tile_position_array[TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * 3];
    for(int i = 0; i < TILEMAP_DIMS; ++i) {
        for(int j = 0; j < TILEMAP_DIMS; ++j) {
            for(int k = 0; k < TILEMAP_LAYERS; ++k) {
                tile_position_array[((TILEMAP_DIMS * TILEMAP_DIMS * k) + (TILEMAP_DIMS*j) + i) * 3] = i;
                tile_position_array[((TILEMAP_DIMS * TILEMAP_DIMS * k) + (TILEMAP_DIMS*j) + i) * 3 + 1] = j;
                tile_position_array[((TILEMAP_DIMS * TILEMAP_DIMS * k) + (TILEMAP_DIMS*j) + i) * 3 + 2] = -k;
            }
        }
    }
    glGenBuffers(1, &tile_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tile_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS * 3 * sizeof(float), tile_position_array, GL_STATIC_DRAW);
    if(checkGLError())
        return 0;

    glGenBuffers(1, &font_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, font_position_buffer);
    if(checkGLError())
        return 0;
    glGenBuffers(1, &font_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, font_buffer);
    if(checkGLError())
        return 0;

    glGenTextures(1, &blank_texture);
    glBindTexture(GL_TEXTURE_2D, blank_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, BLANK_TEXTURE_BUFFER_DATA);
    if(checkGLError())
        return 0;

    viewport_camera = create_camera_2D();
    camera_ortho(viewport_camera, 0, 800, 600, 0, -100, 100);

    return 1;
}

void destroy_graphics()
{
    for(int16_t i = 0; i < loaded_boxes; ++i) {
        glDeleteTextures(1, &boxes[i].texture_data->handle);
        glDeleteBuffers(1, &boxes[i].uv_buffer);
    }
    if(boxes)
        free(boxes);

    delete_program(&sprite_program);
    delete_program(&tile_program);
    delete_program(&font_program);
    delete_program(&single_tile_program);
    delete_program(&box_program);

    glDeleteBuffers(1, &box_buffer);
    glDeleteBuffers(1, &box_s_buffer);
    glDeleteTextures(1, &blank_texture);
    glDeleteBuffers(1, &tile_position_buffer);
    glDeleteBuffers(1, &font_position_buffer);
    glDeleteBuffers(1, &font_buffer);

    cleanup_renderer();
}

void draw_tiles(GLuint tile_buffer, GLuint tile_id_buffer, GLuint tile_set_buffer, mat4 transform, uint16_t x, uint16_t y)
{
    glUseProgram(tile_program.handle);

    const tileset_data* data = tile_settings();

    glEnableVertexAttribArray(tile_vertex_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, get_quad_buffer());
    glVertexAttribPointer(tile_vertex_attrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    checkGLError();

    glEnableVertexAttribArray(tile_position_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, tile_position_buffer);
    glVertexAttribPointer(tile_position_attrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(tile_id_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, tile_id_buffer);
    glVertexAttribIPointer(tile_id_attrib, 1, GL_UNSIGNED_INT, 0, (void*)0);
    checkGLError();
    glEnableVertexAttribArray(tile_tid_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, tile_set_buffer);
    glVertexAttribIPointer(tile_tid_attrib, 1, GL_UNSIGNED_INT, 0, (void*)0);
    checkGLError();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, tile_buffer);
    glUniform1i(tile_texture_uniform, 0);
    checkGLError();

    mat4 st = ident;
    mat4 tt = ident;
    mat4_translate(&tt, data->width * 0.5f + (TILEMAP_DIMS * data->width * x), data->height * 0.5f + (TILEMAP_DIMS * data->height * y), 0);
    mat4_scale(&st, data->width, data->height, 0);
    mat4 final = mat4_mul(camera_get_matrix(viewport_camera), mat4_mul(mat4_mul(transform, tt), st));
    glUniformMatrix4fv(tile_transform_uniform, 1, GL_FALSE, final.data);

    glVertexAttribDivisor(tile_vertex_attrib, 0);
    glVertexAttribDivisor(tile_position_attrib, 1);
    glVertexAttribDivisor(tile_id_attrib, 1);
    glVertexAttribDivisor(tile_tid_attrib, 1);
    checkGLError();

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS);

    glDisableVertexAttribArray(tile_vertex_attrib);
    glVertexAttribDivisor(tile_position_attrib, 0);
    glVertexAttribDivisor(tile_id_attrib, 0);
    glVertexAttribDivisor(tile_tid_attrib, 0);
}

void draw_single_tile(GLuint tile_buffer, GLuint tileset_id, GLuint tile_id, mat4 transform)
{
    glUseProgram(single_tile_program.handle);

    const tileset_data* data = tile_settings();

    glEnableVertexAttribArray(single_tile_vertex_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, get_quad_buffer());
    glVertexAttribPointer(single_tile_vertex_attrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    checkGLError();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, tile_buffer);
    glUniform1i(single_tile_texture_uniform, 0);
    checkGLError();

    glUniform4f(single_tile_color_uniform, 0.9, 0.9, 0.9, 0.7);

    mat4 st = ident;
    mat4 tt = ident;
    mat4_translate(&tt, data->width * 0.5f, data->height * 0.5f, 0);
    mat4_scale(&st, data->width, data->height, 0);
    mat4 final = mat4_mul(camera_get_matrix(viewport_camera), mat4_mul(mat4_mul(transform, tt), st));
    glUniformMatrix4fv(single_tile_transform_uniform, 1, GL_FALSE, final.data);
    glUniform1i(single_tile_id_uniform, tile_id);
    glUniform1i(single_tile_tid_uniform, tileset_id);
    /*info("SET ID: %d", tileset_id);*/

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(single_tile_vertex_attrib);
}

void draw_box(uint16_t id, float x, float y, float w, float h)
{
    glDisable(GL_DEPTH_TEST);
    if(id >= loaded_boxes)
        return;
    ui_box frame = boxes[id];
    glUseProgram(box_program.handle);

    glEnableVertexAttribArray(box_vertex_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, box_buffer);
    glVertexAttribPointer(box_vertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(box_outer_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, box_s_buffer);
    glVertexAttribPointer(box_outer_attrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(box_uv_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, frame.uv_buffer);
    glVertexAttribPointer(box_uv_attrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    checkGLError();

    glUniform3f(box_scale_inner_uniform, w, h, 1);
    glUniform3f(box_scale_full_uniform, w + frame.border_w * 2, h + frame.border_h * 2, 1);
    checkGLError();

    mat4 tt = ident;
    mat4_translate(&tt, x + (w * 0.5) + frame.border_w, y + (h * 0.5) + frame.border_h, 0);
    mat4 final = mat4_mul(camera_get_projection(viewport_camera), tt);
    glUniformMatrix4fv(box_transform_uniform, 1, GL_FALSE, final.data);
    checkGLError();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frame.texture_data->handle); // TODO: Use actual sprite
    glUniform1i(box_texture_uniform, 0);
    checkGLError();

    /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
    glDrawArrays(GL_TRIANGLES, 0, 54);
    checkGLError();
    glEnable(GL_DEPTH_TEST);
}

void draw_text(font* font, const char* text, float x, float y, uint16_t char_count)
{
    glDisable(GL_DEPTH_TEST);
    glUseProgram(font_program.handle);

    glEnableVertexAttribArray(font_vertex_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, get_quad_buffer());
    glVertexAttribPointer(font_vertex_attrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    checkGLError();

     float pos[char_count * 3];
     GLuint ids[char_count];
     float pen_x = 0;
     float pen_y = 0;
     for(uint16_t i = 0; i < char_count; ++i)
     {
        ids[i] = text[i];
        pos[i * 3] = pen_x;
        pos[i * 3 + 1] = pen_y;
        pos[i * 3 + 2] = 0;
        pen_x += 1;
        if(text[i] == '\n') {
            pen_y += 1;
            pen_x = 0;
        }
     }

    
    glBindBuffer(GL_ARRAY_BUFFER, font_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, char_count * 3 * sizeof(float), pos, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, font_buffer);
	glBufferData(GL_ARRAY_BUFFER, char_count * sizeof(GLuint), ids, GL_STREAM_DRAW);
    glEnableVertexAttribArray(font_position_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, font_position_buffer);
    glVertexAttribPointer(font_position_attrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(font_id_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, font_buffer);
    glVertexAttribIPointer(font_id_attrib, 1, GL_UNSIGNED_INT, 0, (void*)0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->texture_data->handle);
    glUniform1i(font_texture_uniform, 0);
    checkGLError();

    mat4 st = ident;
    mat4 tt = ident;
    mat4_translate(&tt, font->glyph_width * 0.5f + x, font->glyph_height * 0.5f + y, 0);
    mat4_scale(&st, font->glyph_width, font->glyph_height, 0);
    mat4 final = mat4_mul(camera_get_projection(viewport_camera), mat4_mul(tt, st));
    glUniformMatrix4fv(font_transform_uniform, 1, GL_FALSE, final.data);

    glVertexAttribDivisor(font_vertex_attrib, 0);
    glVertexAttribDivisor(font_position_attrib, 1);
    glVertexAttribDivisor(font_id_attrib, 1);
    checkGLError();

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, char_count);

    glDisableVertexAttribArray(font_vertex_attrib);
    glVertexAttribDivisor(font_position_attrib, 0);
    glVertexAttribDivisor(font_id_attrib, 0);
    glEnable(GL_DEPTH_TEST);
}

void draw_textbox(font* font, const char* text, uint16_t id, float x, float y, float w, float h, uint16_t char_count)
{
    if(id >= loaded_boxes)
        return;
    ui_box frame = boxes[id];
    draw_box(id, x, y, w, h);
    draw_text(font, text, x + frame.border_w, y + frame.border_h, char_count);
}

void update_camera(float w, float h)
{
    camera_ortho(viewport_camera, 0, w, h, 0, -100, 100);
    view_width = w;
    view_height = h;
}

mat4 get_camera()
{
    return camera_get_matrix(viewport_camera);
}

void update_view(float x, float y, float zoom)
{
    camera_set_position(viewport_camera, x, y, false);
    camera_set_scale(viewport_camera, zoom, zoom, false);
}

void get_viewport_dimensions(uint16_t* w, uint16_t* h)
{
    *w = view_width;
    *h = view_height;
}
