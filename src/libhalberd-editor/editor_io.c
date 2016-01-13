#include "editor_io.h"
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// Hidden variables
///////////////////////////////////////////////////////////////////////////////

actor loaded_actor = 0;
char* actor_path   = 0;
char* actor_name   = 0;

///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

bool init_resource_loader()
{
    set_actor_callback_loader(register_actor_callbacks);

    return true;
}

void destroy_resource_loader()
{
    if(loaded_actor)
        free(loaded_actor);
}

bool register_actor_callbacks(xmlNodePtr ptr, actor a, const char* resource_location, const char* resource_name)
{
    /*register_actor(a, resource_location, resource_name);*/
    // TODO: Get callbacks, and store here
    return true;
}

void register_actor(actor a, const char* resource_location, const char* resource_name)
{
    if(loaded_actor) {
        if(loaded_actor != a) {
            free(loaded_actor);
        }
        free(actor_path);
        free(actor_name);
    }
    loaded_actor = a;
    if(resource_location != 0)
        actor_path = strdup(resource_location);
    actor_name = strdup(resource_name);
}

actor create_blank_actor_for_resource(const char* resource_location, const char* resource_name)
{
    actor a = malloc(sizeof(struct Actor));
    register_actor(a, resource_location, resource_name);

    return a;
}

void save_actor()
{
    // TODO: Error here
    if(!loaded_actor)
        return;
    xmlTextWriterPtr writer = create_xml_resource(actor_path, actor_name);
    xmlTextWriterSetIndent(writer, 2);
    xmlTextWriterStartDocument(writer, NULL, NULL, NULL);
    xmlTextWriterStartElement(writer, "actor");
    fprintf(stderr, "MY DATA: %u\n", loaded_actor->data.flags);
    if(loaded_actor->data.flags & FLAG_SOLID)
        xmlTextWriterWriteAttribute(writer, "solid", "true");
    else
        xmlTextWriterWriteAttribute(writer, "solid", "false");
    if(loaded_actor->data.flags & FLAG_VISIBLE)
        xmlTextWriterWriteAttribute(writer, "visible", "true");
    else
        xmlTextWriterWriteAttribute(writer, "visible", "false");
    if(loaded_actor->data.flags & FLAG_LOCK_TO_GRID)
        xmlTextWriterWriteAttribute(writer, "lock_to_grid", "true");
    else
        xmlTextWriterWriteAttribute(writer, "lock_to_grid", "false");
    if(loaded_actor->data.flags & FLAG_CAN_ORIENT)
        xmlTextWriterWriteAttribute(writer, "can_orient", "true");
    else
        xmlTextWriterWriteAttribute(writer, "can_orient", "false");
    if(loaded_actor->data.flags & FLAG_BLOCK_WITH_SOLID)
        xmlTextWriterWriteAttribute(writer, "block_with_solid", "true");
    else
        xmlTextWriterWriteAttribute(writer, "block_with_solid", "false");
    //--------------------------------------------------------------------------
    // TODO: Move instance-specific data to the map file
    /*
    xmlTextWriterStartElement(writer, "position");
    xmlTextWriterWriteAttribute(writer, "x", loaded_actor->data.position_x);
    xmlTextWriterWriteAttribute(writer, "y", loaded_actor->data.position_y);
    xmlTextWriterEndElement(writer);
    */
    xmlTextWriterStartElement(writer, "display");
    // TODO: Spriteset needs resource information*/
    char id_str[10] = { 0 };
    sprintf(id_str, "%010u", loaded_actor->data.sprites_id);
    xmlTextWriterWriteAttribute(writer, "id", id_str);
    xmlTextWriterEndElement(writer);
    // TODO: Write callback nodes
    //--------------------------------------------------------------------------
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndDocument(writer);

    xmlFreeTextWriter(writer);
}

void set_actor_spriteset_from_resource(const char* resource_location, const char* resource_name)
{
    // TODO: Implement this
}
