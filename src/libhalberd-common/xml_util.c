#include "xml_util.h"
#include "io_util.h"

xmlDocPtr load_resource_to_xml(const char* resource_location, const char* resource_name)
{
    char* path = construct_extended_resource_path(resource_location, resource_name);
    xmlDocPtr doc = xmlReadFile(path, NULL, 0);
    free(path);
    return doc;
}

xmlTextWriterPtr create_xml_resource(const char* resource_location, const char* resource_name)
{
    unsigned char* path = (unsigned char*)construct_extended_resource_path(resource_location, resource_name);
    xmlChar* uri = xmlPathToURI(path);
    xmlTextWriterPtr writer = xmlNewTextWriterFilename((char*)uri, 0);
    free(uri);
    free(path);
    return writer;
}

