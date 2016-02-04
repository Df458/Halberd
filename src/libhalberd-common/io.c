/*#include "io.h"*/
/*#include "util.h"*/

/*#include <png.h>*/
/*#include <stdlib.h>*/
/*#include <libxml/parser.h>*/
/*#include <libxml/tree.h>*/

/*uint8_t load_map(const char* path, tilemap* map)*/
/*{*/
    /*unsigned char* data = load_file(path);*/
    /*if(!data) {*/
        /*warn("No map data found.");*/
        /*return 0;*/
    /*}*/

    /*int i;*/
    /*for(i = 0; i < TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS; ++i) {*/
        /*map->tile_id_data[i] = data[i] - 32;*/
        /*map->tile_set_data[i] = 0;*/
    /*}*/
    /*map->loaded = 1;*/
    /*free(data);*/
    /*return 1;*/
/*}*/

/*void save_map(const char* path, tilemap* map)*/
/*{*/
    /*FILE* file = ex_fopen(path, "wb");*/
    /*if(!file)*/
        /*return NULL;*/
    /*// TODO: This is slow. Try something quicker.*/
    /*int i;*/
    /*for(i = 0; i < TILEMAP_DIMS * TILEMAP_DIMS * TILEMAP_LAYERS; ++i) {*/
        /*fputc(map->tile_id_data[i] + 32, file);*/
    /*}*/
    /*fputc(0, file);*/
    /*fclose(file);*/
/*}*/

/*int16_t load_boxes(const char* path, ui_box** boxes)*/
/*{*/
    /*xmlDocPtr doc = xmlReadFile(path, NULL, 0);*/
    /*if(!doc)*/
        /*return 0;*/
    /*xmlNodePtr root = xmlDocGetRootElement(doc);*/
    /*int16_t count = 0;*/
    /**boxes = malloc(sizeof(ui_box));*/
    /*for(; root; root = root->next)*/
        /*if(root->type == XML_ELEMENT_NODE && !xmlStrcmp(root->name, (const xmlChar*)"box")) {*/
            /*++count;*/
            /*if(count != 1)*/
                /*boxes = realloc(boxes, count * sizeof(ui_box));*/
            /*(*boxes[count - 1]).texture = 0;*/
            /*xmlChar* a = 0;*/
            /*if((a = xmlGetProp(root, (const xmlChar*)"src"))) {*/
                /*(*boxes[count - 1]).texture = load_image((char*)a, &(*boxes[count - 1]).w, &(*boxes[count - 1]).h);*/
                /*free(a);*/
            /*}*/
            /*a = 0;*/
            /*if((a = xmlGetProp(root, (const xmlChar*)"w"))) {*/
                /*(*boxes[count - 1]).border_w = atoi((char*)a);*/
                /*free(a);*/
            /*}*/
            /*a = 0;*/
            /*if((a = xmlGetProp(root, (const xmlChar*)"h"))) {*/
                /*(*boxes[count - 1]).border_h = atoi((char*)a);*/
                /*free(a);*/
            /*}*/

            /*float rx = (float)(*boxes[count - 1]).border_w / (float)(*boxes[count - 1]).w;*/
            /*float ry = (float)(*boxes[count - 1]).border_h / (float)(*boxes[count - 1]).h;*/

            /*float uvs[] = {*/
                /*rx, 0,*/
                /*0, 0,*/
                /*0, ry,*/
                /*rx, 0,*/
                /*0, ry,*/
                /*rx, ry,*/

                /*1, 0,*/
                /*1 - rx, 0,*/
                /*1 - rx, ry,*/
                /*1, 0,*/
                /*1 - rx, ry,*/
                /*1, ry,*/

                /*rx, 1 - ry,*/
                /*0,  1 - ry,*/
                /*0,  1,*/
                /*rx, 1 - ry,*/
                /*0,  1,*/
                /*rx, 1,*/

                /*1, 1 - ry,*/
                /*1 - rx, 1 - ry,*/
                /*1 - rx, 1,*/
                /*1, 1 - ry,*/
                /*1 - rx, 1,*/
                /*1, 1,*/

                /*rx, ry,*/
                /*0, ry,*/
                /*0, 1 - ry,*/
                /*rx, ry,*/
                /*0, 1 - ry,*/
                /*rx, 1 - ry,*/

                /*1, ry,*/
                /*1 - rx, ry,*/
                /*1 - rx, 1 - ry,*/
                /*1, ry,*/
                /*1 - rx, 1 - ry,*/
                /*1, 1 - ry,*/

                /*1 - rx, 0,*/
                /*rx, 0,*/
                /*rx, ry,*/
                /*1 - rx, 0,*/
                /*rx, ry,*/
                /*1 - rx, ry,*/

                /*1 - rx, 1 - ry,*/
                /*rx, 1 - ry,*/
                /*rx, 1,*/
                /*1 - rx, 1 - ry,*/
                /*rx, 1,*/
                /*1 - rx, 1,*/

                /*1 - rx, ry,*/
                /*rx, ry,*/
                /*rx, 1 - ry,*/
                /*1 - rx, ry,*/
                /*rx, 1 - ry,*/
                /*1 - rx, 1 - ry,*/
            /*};*/
            /*glGenBuffers(1, &(*boxes[count - 1]).uvs);*/
            /*glBindBuffer(GL_ARRAY_BUFFER, (*boxes[count - 1]).uvs);*/
            /*glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), uvs, GL_STATIC_DRAW);*/

            /*if((*boxes[count - 1]).texture == 0)*/
                /*--count;*/
        /*}*/

    /*xmlFreeDoc(doc);*/
    /*xmlCleanupParser();*/
    /*if(count == 0)*/
        /*return -1;*/
    /*return count;*/
/*}*/
