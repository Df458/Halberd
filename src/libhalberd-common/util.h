#ifndef UTIL_H
#define UTIL_H

#include <errno.h>
#include <inttypes.h>
#include <string.h>

void _warn (const char* file, unsigned line, char* const message, ...);
void _error(const char* file, unsigned line, char* const message, ...);
#define warn(message, ...) _warn(__FILE__, __LINE__, message, ## __VA_ARGS__)
#define error(message, ...) _error(__FILE__, __LINE__, message, ## __VA_ARGS__)
#define error_code() _error(__FILE__, __LINE__, strerror(errno))
#define nulltest(ptr) if(!ptr) error("Recieved unexpected null pointer")

unsigned char* loadFileContents(const char* const filepath);

#define DEGTORAD 0.01745329251
#define RADTODEG 57.2957795131

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
