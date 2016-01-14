#include "util.h"

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
}

void _error(const char* file, unsigned line, char* const message, ...)
{
    fprintf(stderr, "%s:%d, FATAL ERROR: %s\n", file, line, message);
    exit(1);
}
