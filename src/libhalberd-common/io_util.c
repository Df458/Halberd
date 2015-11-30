#include "io_util.h"
#include "util.h"

#include <stdlib.h>
#ifdef WINDOWS
#include <windows.h>
#elif __GNUC__
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/stat.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Hidden variables
///////////////////////////////////////////////////////////////////////////////

char* resource_path = NULL;

///////////////////////////////////////////////////////////////////////////////
// Hidden functions
///////////////////////////////////////////////////////////////////////////////

void get_exe_path(char* buf)
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

///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

const char* get_base_resource_path()
{
    if(resource_path == NULL) {
        resource_path = calloc(512, sizeof(char));
        get_exe_path(resource_path);
    }

    return resource_path;
}

char* construct_extended_resource_path(const char* resource_location, const char* resource_name)
{
    nulltest(resource_name);
    size_t len = strlen(resource_path) + strlen(resource_name) + 2;
    char* path = NULL;

    if(resource_location != NULL) {
        len += strlen(resource_location) + 1;
        path = calloc(len, sizeof(char));
        strcat(path, resource_path);
        strcat(path, "/");
        strcat(path, resource_location);
        strcat(path, "/");
        strcat(path, resource_name);
    } else {
        path = calloc(len, sizeof(char));
        strcat(path, resource_path);
        strcat(path, "/");
        strcat(path, resource_name);
    }

    return path;
}

void set_resource_path(const char* path)
{
    nulltest(path);
    if(resource_path != NULL)
        free(resource_path);
    resource_path = strdup(path);
}

void set_resource_path_relative(const char* path)
{
    nulltest(path);
    if(resource_path != NULL)
        free(resource_path);

    resource_path = calloc(512 + strlen(path), sizeof(char));
    get_exe_path(resource_path);
    strcat(resource_path, "/");
    strcat(resource_path, path);
}

FILE* load_resource_file(const char* resource_location, const char* resource_name, const char* mode)
{
    char* final_path = construct_extended_resource_path(resource_location, resource_name);
    FILE* file = fopen(final_path, mode);
    free(final_path);
    if(!file) {
        fprintf(stderr, "%s not found\n", final_path);
        error("Failed to load file: File not found.");
        return NULL;
    }

    return file;
}

unsigned char* load_resource_to_buffer(const char* resource_location, const char* resource_name)
{
    FILE* file = load_resource_file(resource_location, resource_name, "rb");
    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file) + 1;
    fseek(file, 0, SEEK_SET);
    unsigned char* filedata = calloc(filesize, sizeof(unsigned char));
    fread(filedata, 1, filesize, file);
    filedata[filesize - 1] = '\0';

    fclose(file);
    return filedata;
}

bool ensure_directory(const char* path)
{
    struct stat dir_stat = {0};
    if (stat(path, &dir_stat) != -1) {
        return false;
    }

    mkdir(path, 0700);
    return true;
}

char* swap_extension(const char* file_name, const char* file_extension)
{
    const char* ext_ptr = strchr(file_name, (int)'.');
    char* new_name = NULL;
    if(file_extension != NULL) {
        new_name = calloc(strlen(file_name) - strlen(ext_ptr) + strlen(file_extension) + 2, sizeof(char));
        strncpy(new_name, file_name, strlen(file_name) - strlen(ext_ptr));
        strcat(new_name, ".");
        strcat(new_name, file_extension);
    } else {
        new_name = calloc(strlen(file_name) - strlen(ext_ptr) + 1, sizeof(char));
        strncpy(new_name, file_name, strlen(file_name) - strlen(ext_ptr));
    }

    return new_name;
}
