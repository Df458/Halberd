#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <libxml/uri.h>
#include <libxml/xmlwriter.h>

#define PROJECT_FILE_VERSION "0.1.0"

char* project_name;
char* directory_path;

bool create_project(const char* path)
{
    // Check to see if the project directory exists already
    struct stat dir_stat = {0};
    if (stat(path, &dir_stat) != -1) {
        return false;
    }

    mkdir(path, 0750);

    char* content_path = calloc(strlen(path) + 8 + 1, sizeof(char));
    strcpy(content_path, path);
    content_path = strcat(content_path, "/Content");
    fprintf(stderr, "PATH: %s\nCPATH: %s\n", path, content_path);
    mkdir(content_path, 0750);

    char* project_name = strrchr(path, '/') + 1;
    
    char* project_path = calloc(strlen(path) + strlen(project_name) + 5 + 1, sizeof(char));
    strcpy(project_path, path);
    project_path = strcat(project_path, "/");
    project_path = strcat(project_path, project_name);
    project_path = strcat(project_path, ".hal");
    fprintf(stderr, "NAME: %s\nPPATH: %s\n", project_name, project_path);
    // Create the project file
    xmlChar* uri = xmlPathToURI(project_path);
    xmlTextWriterPtr writer = xmlNewTextWriterFilename(uri, 0);
    xmlTextWriterSetIndent(writer, 2);

    // TODO: Write the file here
    xmlTextWriterStartDocument(writer, NULL, NULL, NULL);
    xmlTextWriterStartElement(writer, "project");
    xmlTextWriterWriteAttribute(writer, "version", PROJECT_FILE_VERSION);
    //--------------------------------------------------------------------------
    xmlTextWriterStartElement(writer, "start");
    // TODO: Write initial map and start script info here
    xmlTextWriterEndElement(writer);
    xmlTextWriterStartElement(writer, "settings");
    // TODO: Write settings here
    xmlTextWriterEndElement(writer);
    //--------------------------------------------------------------------------
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndDocument(writer);

    xmlFreeTextWriter(writer);
    free(uri);
    // TODO: This seems to cause memory errors. May be unnecessary, and should
    //       be looked into.
    free(content_path);
    free(project_path);
}

bool load_project(const char* path)
{
    const char* title_ptr = strrchr(path, '/') + 1;
    project_name = calloc(strlen(title_ptr) - 4, sizeof(char));
    directory_path = calloc(strlen(path) - strlen(title_ptr), sizeof(char));
    strncpy(directory_path, path, strlen(path) - strlen(title_ptr));
    strncpy(project_name, title_ptr, strlen(title_ptr) - 4);
    fprintf(stderr, "Load Path: %s\nLoad Name: %s\n", directory_path, project_name);
}

const char* get_project_name()
{
    return project_name;
}

const char* get_project_path()
{
    return directory_path;
}
