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

void _warn (const char* file, unsigned line, char* const message)
{
    fprintf(stderr, "%s:%d, WARNING: %s\n", file, line, message);
}

void _error(const char* file, unsigned line, char* const message)
{
    fprintf(stderr, "%s:%d, FATAL ERROR: %s\n", file, line, message);
    exit(1);
}

unsigned char* loadFileContents(const char* const filepath)
{
    FILE* file = fopen(filepath, "rb");
    if(!file) {
        fprintf(stderr, "%s not found\n", filepath);
        error("Failed to load file: File not found.");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    unsigned char* filedata = calloc(filesize, sizeof(char));
    fread(filedata, 1, filesize, file);
    filedata[filesize - 1] = '\0';

    fclose(file);
    return filedata;
}

void get_path(char* buf)
{
#ifdef WINDOWS
    GetModuleFileName(NULL, buf, strlen(buf));
    char* c = strrchr(buf, '/');
    c[1] = 0;
#elif __GNUC__
    ssize_t len = readlink("/proc/self/exe", buf, strlen(buf) - 1);
    if (len != -1) {
        char* c = strrchr(buf, '/');
        c[1] = 0;
    } else {
        error_code();
    }
#endif
}
