#ifndef CALLBACKS_H
#define CALLBACKS_H
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <libxml/tree.h>
#include "actor.h"

void build_actor_states(actor* a, xmlNodePtr node);

int lua_animation_play(lua_State* state);
int lua_message(lua_State* state);

static const luaL_Reg actor_funcs[] =
{
    {"animation_play", lua_animation_play},
    {"message", lua_message},
    {0, 0}
};

#endif
