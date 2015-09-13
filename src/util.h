#ifndef UTIL_H
#define UTIL_H

#include <errno.h>
#include <inttypes.h>
#include <string.h>

void _warn (const char* file, unsigned line, char* const message);
void _error(const char* file, unsigned line, char* const message);
#define warn(message) _warn(__FILE__, __LINE__, message)
#define error(message) _error(__FILE__, __LINE__, message)
#define error_code() _error(__FILE__, __LINE__, strerror(errno))

unsigned char* loadFileContents(const char* const filepath);
void getPath(char* buf);

#define DEGTORAD 0.01745329251
#define RADTODEG 57.2957795131

#endif
