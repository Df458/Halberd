#ifndef RENDER_H
#define RENDER_H
#include "map.h"
#include <dfgame-common.h>
#include <stdbool.h>

static const float GRID_BUFFER_DATA[] =
{
// Vertex positon/uv
0, 0, 0,
TILEMAP_DIMS * 32, TILEMAP_DIMS * 32, 0,
};

static const float GRID_TEXTURE_BUFFER_DATA[] =
{
    1.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 0.0f,
};

static const char* GRID_VERTEX_SHADER[] =
{"# version 330\n"
"layout(location=0) in vec3 vertex_pos;\n"
"layout(location=1) in vec3 position;\n"
"layout(location=2) in int horizontal;\n"

"uniform mat4 transform;\n"
"out vec2 uv;\n"
"void main() {\n"
"vec2 hvec = vec2(horizontal, 1 - horizontal);\n"
"uv = vec2(0, vertex_pos.y);\n"
"gl_Position = transform * vec4((vertex_pos * vec3(hvec, 1)) + position, 1.0);\n"
"}"};

static const char* GRID_FRAGMENT_SHADER[] =
{"# version 330\n"
"in vec2 uv;\n"
"uniform sampler2D texture;\n"
"uniform vec4 color = vec4(0, 0, 0, 0.5);\n"

"layout(location=0) out vec4 fragcolor;\n"
"void main() {\n"
"fragcolor = texture2D(texture, uv) * color;\n"
//"if(fragcolor.a == 0)\n"
//"discard;\n"
"}"};

bool init_render();
bool render_editor();
void destroy_render();
bool init_grid();
void draw_grid(mat4 transform);
void size_callback(int width, int height);
#endif
