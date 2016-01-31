#include <string.h>
#include "actor.h"
#include "asset_registry.h"

///////////////////////////////////////////////////////////////////////////////
// Hidden variables
///////////////////////////////////////////////////////////////////////////////

callback_loader load_callback_fn;

///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

void set_actor_callback_loader(callback_loader loader)
{
    load_callback_fn = loader;
}

actor load_actor_from_resource(const char* resource_location, const char* resource_name)
{
    xmlDocPtr doc = load_resource_to_xml(resource_location, resource_name);
    if(!doc)
        return 0;
    xmlNodePtr root = xmlDocGetRootElement(doc);
    for(; root; root = root->next)
        if(root->type == XML_ELEMENT_NODE && !xmlStrcmp(root->name, (const xmlChar*)"actor"))
            break;
    if(!root)
        return 0;
    actor a_new = malloc(sizeof(struct Actor));
    a_new->data.grid_x = 0;
    a_new->data.grid_y = 0;
    a_new->data.super_grid_x = 0;
    a_new->data.super_grid_y = 0;
    a_new->data.flags = 0;
    a_new->data.sprites = 0;
    memset(a_new->callbacks, 0, CALLBACK_COUNT * sizeof(lua_State*));
    a_new->data.speed = 0;
    a_new->data.moving = 0;
    a_new->data.orientation = 0;
    a_new->data.animation_index = 0;
    a_new->data.animation_playing = 0;
    a_new->data.animation_timer = 0;
    a_new->data.sprites_id = UINT32_MAX;

    for(xmlNodePtr node = root->children; node; node = node->next) {
        if(node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar*)"position")) {
            xmlChar* a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"x"))) {
                a_new->data.grid_x = atoi((char*)a);
                free(a);
            }
            a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"y"))) {
                a_new->data.grid_y = atoi((char*)a);
                free(a);
            }
        }
        if(node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar*)"display")) {
            xmlChar* a = 0;
            if((a = xmlGetProp(node, (const xmlChar*)"id"))) {
                a_new->data.sprites_id = atoi((char*)a);
                a_new->data.sprites = (sprite*)get_data_from_id(a_new->data.sprites_id);/*load_resource_to_spriteset("sprites", (char*)a);*/
                free(a);
            }
        }
        if(node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar*)"callback")) {
            load_callback_fn(node, a_new, resource_location, resource_name);
        }
    }
    xmlChar* a = 0;
    if((a = xmlGetProp(root, (const xmlChar*)"solid"))) {
        a_new->data.flags += strcmp((char*)a, "false") ? FLAG_SOLID : 0;
        free(a);
    }
    if((a = xmlGetProp(root, (const xmlChar*)"visible"))) {
        a_new->data.flags += strcmp((char*)a, "false") ? FLAG_VISIBLE : 0;
        free(a);
    }
    if((a = xmlGetProp(root, (const xmlChar*)"lock_to_grid"))) {
        a_new->data.flags += strcmp((char*)a, "false") ? FLAG_LOCK_TO_GRID : 0;
        free(a);
    }
    if((a = xmlGetProp(root, (const xmlChar*)"can_orient"))) {
        a_new->data.flags += strcmp((char*)a, "false") ? FLAG_CAN_ORIENT : 0;
        free(a);
    }
    if((a = xmlGetProp(root, (const xmlChar*)"block_with_solid"))) {
        a_new->data.flags += strcmp((char*)a, "false") ? FLAG_BLOCK_WITH_SOLID : 0;
        free(a);
    }

    if(a_new->data.sprites != 0) {
        a_new->data.animation_index = 0;
        a_new->data.animation_playing = a_new->data.sprites->animations[0].play;
    }
    a_new->data.position_x = a_new->data.grid_x * TILE_WIDTH;
    a_new->data.position_y = a_new->data.grid_y * TILE_HEIGHT;

    xmlFreeDoc(doc);
    /*xmlCleanupParser();*/

    return a_new;
}
