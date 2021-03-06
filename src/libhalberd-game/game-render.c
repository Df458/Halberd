#include <dfgame-common.h>
#include <GL/glew.h>
#include "game-render.h"
#include "render_util.h"
#include "player.h"
#include "actor-manager.h"
#include "ui.h"
#include "map.h"

bool render_game()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mat4 t = ident;
    draw_maps(t);

    draw_player();
    draw_actors();
    draw_ui();
    return true;
}
