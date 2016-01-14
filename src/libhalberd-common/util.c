#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WINDOWS
#include <windows.h>
#elif __GNUC__
#define _GNU_SOURCE
#include <unistd.h>
#endif

// TODO: Use the extra variables
void _warn (const char* file, unsigned line, char* const message, ...)
{
    fprintf(stderr, "%s:%d, WARNING: %s\n", file, line, message);
	va_list args;
	size_t length;

    // Get the length of the format string
	length = snprintf(0, 0, "%s:%d, WARNING: %s\n", file, line, message);
    ++length;
    char* format = malloc(sizeof(char) * length);
    if(!format) {
        fprintf(stderr, "FATAL ERROR: Cannot write message: Out of memory\n Partial message text: %s\n", message);
        exit(1);
    }
    snprintf(format, length - 1, "%s:%d, WARNING: %s\n", file, line, message);

	va_start(args, message);
    vfprintf(stderr, format, args);
	va_end(args);
}

void _error(const char* file, unsigned line, char* const message, ...)
{
    fprintf(stderr, "%s:%d, FATAL ERROR: %s\n", file, line, message);
	va_list args;
	size_t length;

    // Get the length of the format string
	length = snprintf(0, 0, "%s:%d, FATAL ERROR: %s\n", file, line, message);
    ++length;
    char* format = malloc(sizeof(char) * length);
    if(!format) {
        fprintf(stderr, "FATAL ERROR: Cannot write message: Out of memory\n Partial message text: %s\n", message);
        exit(1);
    }
    snprintf(format, length - 1, "%s:%d, FATAL ERROR: %s\n", file, line, message);

	va_start(args, message);
    vfprintf(stderr, format, args);
	va_end(args);

    exit(1);
}
