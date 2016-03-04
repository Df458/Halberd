#include <string.h>
#include "callbacks.h"
#include "actor.h"
#include "dfgame-common.h"
#include "ui.h"

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
            char* file = (char*)load_resource_to_buffer("scripts", (char*)b);
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

    sprite_set_playing((*actor)->data.sprites, true);

    return 0;
}

int lua_message(lua_State* state)
{
    add_message(lua_tostring(state, -1));
    return 0;
}
