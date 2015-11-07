#include <string.h>
#include "callbacks.h"
#include "actor.h"
#include "io.h"
#include "ui.h"

/*struct actordata*/
/*{*/
    /*uint32_t position_x;*/
    /*uint32_t position_y;*/
    /*int16_t grid_x;*/
    /*int16_t grid_y;*/
    /*int16_t super_grid_x;*/
    /*int16_t super_grid_y;*/
    /*uint16_t speed;*/
    /*uint8_t moving;*/
    /*uint8_t orientation;*/
    /*uint8_t flags;*/
    /*int8_t animation_index;*/
    /*uint8_t animation_playing;*/
    /*float animation_timer;*/

    /*spriteset* sprites;*/
/*};*/

/*typedef struct Actor*/
/*{*/
    /*struct actordata data;*/
    /*lua_State* callbacks[CALLBACK_COUNT];*/
/*} Actor;*/

void build_actor_states(actor a_new, xmlNodePtr node)
{
    xmlChar* a = 0;
    if((a = xmlGetProp(node, (const xmlChar*)"type"))) {
        xmlChar* b = 0;
        int8_t cid = -1;
        for(int i = 0; i < CALLBACK_COUNT; ++i)
            if(!strcmp((char*)a, CALLBACK_NAMES[i])) {
                cid = i;
            }
        if(cid == -1)
            return;
        a_new->callbacks[cid] = luaL_newstate();
        luaL_openlibs(a_new->callbacks[cid]);
        if((b = xmlGetProp(node, (const xmlChar*)"src"))) {
            char* file = (char*)load_file((char*)b);
            luaL_loadstring(a_new->callbacks[cid], file);
            lua_setglobal(a_new->callbacks[cid], CALLBACK_NAMES[cid]);
            free(file);

            lua_newtable(a_new->callbacks[cid]);
            luaL_setfuncs(a_new->callbacks[cid], actor_funcs, 0);
            actor* actordat = (actor*)lua_newuserdata(a_new->callbacks[cid], sizeof(actor));
            *actordat = a_new;
            lua_setfield(a_new->callbacks[cid], 1, "instance");
            lua_setglobal(a_new->callbacks[cid], "this");
            free(b);
        }
        free(a);
    }
}

int lua_animation_play(lua_State* state)
{
    lua_getfield(state, 1, "instance");
    if(!lua_isuserdata(state, -1))
        return luaL_error(state, "Trying to access an actor, but the instance is missing!");
    actor* actor = lua_touserdata(state, -1);
    lua_pop(state, 1);

    (*actor)->data.animation_playing = 1;

    return 0;
}

int lua_message(lua_State* state)
{
    add_message(lua_tostring(state, -1));
    return 0;
}
