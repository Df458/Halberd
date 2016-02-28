#ifndef RENDER_UTIL
#define RENDER_UTIL
#include <dfgame-common.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <inttypes.h>
#include "color.h"
#include "texture_util.h"

#define TILE_WIDTH 32
#define TILE_HEIGHT 32

enum RenderPass
{
    FIRST_PASS = 0,
    STATIC_PASS = FIRST_PASS,
    DYNAMIC_PASS,
    TRANSPARENT_PASS,
    SKY_PASS,
    HIDDEN_PASS,
    UI_PASS,
    LAST_PASS
};

static const char* SPRITE_VERTEX_SHADER[] =
{"# version 120\n"
"attribute vec3 vertex_pos;\n"
"uniform mat4 transform;\n"
"varying vec2 uv;\n"
"uniform vec2 uv_offset = vec2(0, 0);\n"
"uniform vec2 uv_dimensions = vec2(1, 1);\n"
"void main() {\n"
"uv = ((vertex_pos.xy + 0.5f) * -1 + 1) * uv_dimensions + uv_offset;\n"// TODO: Might just want to add uvs to the buffer, it's likely cheaper in the long run
"gl_Position = transform * vec4(vertex_pos, 1.0);\n"
"}"};

static const char* SPRITE_FRAGMENT_SHADER[] =
{"# version 120\n"
"uniform vec4 color;\n"
"uniform sampler2D texture;\n"
"varying vec2 uv;\n"
"void main() {\n"
"gl_FragColor = texture2D(texture, uv) * color;\n"
"if(gl_FragColor.a == 0)\n"
"discard;\n"
"}"};

// TODO: Try and combine these shaders using a "single-side-length" uniform
static const char* TILE_VERTEX_SHADER[] =
{"# version 330\n"
"layout(location=0) in vec3 vertex_pos;\n"
"layout(location=1) in vec3 position;\n"
"layout(location=2) in int id_in;\n"
"layout(location=3) in int tid_in;\n"

"uniform mat4 transform;\n"
"out vec3 uvw;\n"
"flat out int id;\n"
"void main() {\n"
//"uv = (vertex_pos.xy + 0.5f) * -1 + 1;\n"
"uvw.xy = vertex_pos.xy + vec2(0.5, 0.5);\n"
//"uv.x = uv.x + (id_in - uint(1)) % uint(8);"
"int row = (id_in - int(1)) % int(32);\n"
"int column = (id_in - int(1)) / int(32);\n"
"uvw.x = uvw.x / 32 + (float(row) / 32);\n"
"uvw.y = 1 - (uvw.y / 32 + float(column) / 32);\n"
"uvw.z = float(tid_in);\n"
"id = id_in;\n"
"gl_Position = transform * vec4(vertex_pos + position, 1.0);\n"
"}"};

static const char* SINGLE_TILE_VERTEX_SHADER[] =
{"# version 330\n"
"layout(location=0) in vec3 vertex_pos;\n"
"uniform mat4 transform;\n"
"uniform int id_in;\n"
"uniform int tid_in;\n"
"flat out int id;\n"
"out vec3 uvw;\n"
"void main() {\n"
//"uv = (vertex_pos.xy + 0.5f) * -1 + 1;\n"
"uvw.xy = vertex_pos.xy + vec2(0.5, 0.5);\n"
//"uv.x = uv.x + (id_in - uint(1)) % uint(8);"
"int row = (id_in - int(1)) % int(32);\n"
"int column = (id_in - int(1)) / int(32);\n"
"uvw.x = uvw.x / 32 + (float(row) / 32);\n"
"uvw.y = 1 - (uvw.y / 32 + float(column) / 32);\n"
"uvw.z = float(tid_in);\n"
"id = id_in;\n"
"gl_Position = transform * vec4(vertex_pos, 1.0);\n"
"}"};

static const char* TILE_FRAGMENT_SHADER[] =
{"# version 330\n"
"in vec3 uvw;\n"
"flat in int id;\n"
"uniform sampler2DArray tex;\n"
"uniform vec4 color = vec4(1);\n"

"layout(location=0) out vec4 fragcolor;\n"
"void main() {\n"
"fragcolor = texture(tex, uvw) * color;\n"
"if(fragcolor.a == 0 || id == int(0))\n"
"discard;\n"
"}"};

static const char* FONT_VERTEX_SHADER[] =
{"# version 330\n"
"layout(location=0) in vec3 vertex_pos;\n"
"layout(location=1) in vec3 position;\n"
"layout(location=2) in int id_in;\n"

"uniform mat4 transform;\n"
"out vec2 uv;\n"
"flat out int id;\n"
"void main() {\n"
"uv = vertex_pos.xy + vec2(0.5, 0.5);\n"
"int row = id_in % int(16);\n"
"int column = id_in / int(16);\n"
"uv.x = uv.x / 16 + (float(row) / 16);\n"
"uv.y = 1 - (uv.y / 16 + float(column) / 16);\n"
"id = id_in;\n"
"gl_Position = transform * vec4(vertex_pos + position, 1.0);\n"
"}"};

static const char* FONT_FRAGMENT_SHADER[] =
{"# version 330\n"
"in vec2 uv;\n"
"flat in int id;\n"
"uniform sampler2D tex;\n"
"uniform vec4 color = vec4(1);\n"

"layout(location=0) out vec4 fragcolor;\n"
"void main() {\n"
"fragcolor = texture(tex, uv) * color;\n"
"if(fragcolor.a == 0 || id == int(0))\n"
"discard;\n"
"}"};

static const char* BOX_VERTEX_SHADER[] =
{"# version 330\n"
"in vec3 vertex_pos;\n"
"in vec2 vertex_uv;\n"
"in vec2 vertex_rim;\n"
"out vec2 uv;\n"
"uniform vec3 scale_inner;\n"
"uniform vec3 scale_full;\n"
"uniform mat4 transform;\n"
"void main() {\n"
"gl_Position = transform * vec4((vertex_pos * scale_inner * vec3(1-vertex_rim.x, 1-vertex_rim.y, 1)) + (vertex_pos * scale_full * vec3(vertex_rim, 1)), 1);\n"
"uv = vertex_uv;\n"
"}"};

static const char* BOX_FRAGMENT_SHADER[] =
{"# version 330\n"
"in vec2 uv;\n"
"uniform sampler2D texture;\n"
"uniform vec4 color = vec4(1);\n"
"layout(location=0) out vec4 fragcolor;\n"
"void main() {\n"
"fragcolor = texture2D(texture, uv) * color;\n"
"if(fragcolor.a == 0)\n"
"discard;\n"
"}"};

//static const float QUAD_BUFFER_DATA[] =
//{
//// Vertex positon/uv
//0.5, -0.5, 0,
//-0.5, -0.5, 0,
//-0.5, 0.5, 0,

//0.5, -0.5, 0,
//-0.5, 0.5, 0,
//0.5, 0.5, 0,
//};

static const float BOX_S_BUFFER_DATA[] =
{
//ul
0, 1,
1, 1,
1, 0,
      
0, 1,
1, 0,
0, 0,


//ur
1, 1,
0, 1,
0, 0,
      
1, 1,
0, 0,
1, 0,


//bl
0, 0,
1, 0,
1, 1,
      
0, 0,
1, 1,
0, 1,


//br
1, 0,
0, 0,
0, 1,
     
1, 0,
0, 1,
1, 1,


//l
0, 0,
1, 0,
1, 0,
     
0, 0,
1, 0,
0, 0,


//r
1, 0,
0, 0,
0, 0,
     
1, 0,
0, 0,
1, 0,


//t
0, 1,
0, 1,
0, 0,
     
0, 1,
0, 0,
0, 0,


//b
0, 0,
0, 0,
0, 1,
      
0, 0,
0, 1,
0, 1,


//c
0, 0,
0, 0,
0, 0,
     
0, 0,
0, 0,
0, 0,
};

static const float BOX_BUFFER_DATA[] = // TODO: Make UVs changeable
{
// Vertex positon/scaler, upper-left
-0.5, -0.5, 0, 
-0.5, -0.5, 0, 
-0.5, -0.5, 0, 
       
-0.5, -0.5, 0, 
-0.5, -0.5, 0, 
-0.5, -0.5, 0, 
// Vertex positon/scaler, upper-right
 0.5, -0.5, 0, 
 0.5, -0.5, 0, 
 0.5, -0.5, 0, 

 0.5, -0.5, 0, 
 0.5, -0.5, 0, 
 0.5, -0.5, 0, 
// Vertex positon/scaler, bottom-left
-0.5,  0.5, 0, 
-0.5,  0.5, 0, 
-0.5,  0.5, 0, 

-0.5,  0.5, 0, 
-0.5,  0.5, 0, 
-0.5,  0.5, 0, 
// Vertex positon/scaler, bottom-right
 0.5,  0.5, 0, 
 0.5,  0.5, 0, 
 0.5,  0.5, 0, 

 0.5,  0.5, 0, 
 0.5,  0.5, 0, 
 0.5,  0.5, 0, 
// Vertex positon/scaler, left
-0.5, -0.5, 0, 
-0.5, -0.5, 0, 
-0.5,  0.5, 0, 

-0.5, -0.5, 0, 
-0.5,  0.5, 0, 
-0.5,  0.5, 0, 
// Vertex positon/scaler, right
 0.5, -0.5, 0, 
 0.5, -0.5, 0, 
 0.5,  0.5, 0, 

 0.5, -0.5, 0, 
 0.5,  0.5, 0, 
 0.5,  0.5, 0, 
// Vertex positon/scaler, top
 0.5, -0.5, 0, 
-0.5, -0.5, 0, 
-0.5, -0.5, 0, 

 0.5, -0.5, 0, 
-0.5, -0.5, 0, 
 0.5, -0.5, 0, 
// Vertex positon/scaler, bottom
 0.5,  0.5, 0, 
-0.5,  0.5, 0, 
-0.5,  0.5, 0, 

 0.5,  0.5, 0, 
-0.5,  0.5, 0, 
 0.5,  0.5, 0, 
// Vertex positon/scaler, center
 0.5, -0.5, 0, 
-0.5, -0.5, 0, 
-0.5,  0.5, 0, 

 0.5, -0.5, 0, 
-0.5,  0.5, 0, 
 0.5,  0.5, 0, 
};

static const float BLANK_TEXTURE_BUFFER_DATA[] =
{
    1.0f, 1.0f, 1.0f, 1.0f,
};

uint8_t init_graphics(void);
void destroy_graphics();

void draw_sprite(sprite* spr, uint8_t a_index, uint8_t f_index, uint8_t o_index, uint8_t o_count, float position_x, float position_y, float rotation, float scale_x, float scale_y, color col);
void draw_tiles(GLuint tile_buffer, GLuint tile_id_buffer, GLuint tile_set_buffer, mat4 transform, uint16_t x, uint16_t y);
void draw_single_tile(GLuint tile_buffer, GLuint tileset_id, GLuint tile_id, mat4 transform);
void draw_box(uint16_t id, float x, float y, float w, float h);
void draw_text(font* font, const char* text, float x, float y, uint16_t char_count);
void draw_textbox(font* font, const char* text, uint16_t id, float x, float y, float w, float h, uint16_t char_count);

void update_camera(float w, float h);
void update_view(float x, float y, float zoom);
mat4 get_camera();
mat4 get_view();
void get_viewport_dimensions(uint16_t* w, uint16_t* h);

#endif
