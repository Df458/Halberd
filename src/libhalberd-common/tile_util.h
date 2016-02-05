#ifndef TILE_UTIL_H
#define TILE_UTIL_H
#include <inttypes.h>

static const int8_t surround_x_table[] =
{
    -1,
     0,
     1,
    -1,
     1,
    -1,
     0,
     1
};

static const int8_t surround_y_table[] =
{
    -1,
    -1,
    -1,
     0,
     0,
     1,
     1,
     1
};

#endif
