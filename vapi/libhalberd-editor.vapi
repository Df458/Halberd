[CCode (cheader_filename = "editor-render.h", cheader_filename = "project.h")]
namespace Halberd {
namespace Editor {
    [CCode (cname="init_render")]
    bool init_render();
    [CCode (cname="render_editor")]
    bool render();
    [CCode (cname="destroy_render")]
    void cleanup_render();
    [CCode (cname="size_callback")]
    bool size_callback(int w, int h);
    [CCode (cname="create_project")]
    bool create_project(string path);
    [CCode (cheader_filename = "autotile.h")]
    namespace Automaps {
        [CCode (cname="init_automaps")]
        void init();
        [CCode (cname="clear_automaps")]
        void clear();
        [CCode (cname="load_automaps")]
        void load(string filepath);
        [CCode (cname="save_automaps")]
        void save();
    }
    [CCode (cheader_filename="cursor.h")]
    namespace Cursor {
        [CCode (cname="cursor_init")]
        void init();
        [CCode (cname="cursor_set_position")]
        void set_position(int x, int y);
        [CCode (cname="map_set_position")]
        void set_map_position(int x, int y);
        [CCode (cname="map_drag")]
        void drag_map(int x, int y);
        [CCode (cname="cursor_zoom")]
        void zoom(double input);
        [CCode (cname="cursor_place_tile")]
        void place_tile(ushort tile);
        [CCode (cname="cursor_place_line")]
        void place_line(ushort tile, int x, int y);
        [CCode (cname="cursor_flood_fill")]
        void flood_fill(ushort tile);
    }
}
}
