[CCode (cheader_filename = "actor.h", cheader_filename = "gamer-render.h")]
namespace Halberd {
namespace Game {
    [CCode (cname="render_game")]
    bool render();

    [CCode (cname = "int", cprefix = "ORIENT_", has_type_id = false)]
    public enum Orientation {
        NORTH,
        EAST,
        SOUTH,
        WEST,
        NORTHEAST,
        SOUTHEAST,
        NORTHWEST,
        SOUTHWEST
    }

    [CCode (cname = "int", cprefix = "FLAG_", has_type_id = false)]
    [Flags]
    public enum ActorFlag {
        SOLID,
        VISIBLE,
        LOCK_TO_GRID,
        CAN_ORIENT,
        BLOCK_WITH_SOLID
    }

    [CCode (cname = "callback_types", cprefix = "CALLBACK_", has_type_id = false)]
    public enum Callback {
        CREATED,
        DESTROYED,
        COLLISION,
        TICK,
        OPERATED,
    }

    [CCode (cheader_filename = "map.h")]
    namespace Maps {
        [CCode (cname = "init_maps")]
        void init();
        [CCode (cname = "load_maps_from_resource")]
        void load(string? location, string name);
        [CCode (cname = "save_maps_to_resource")]
        void save(string? location, string name);
        [CCode (cname = "add_tileset_to_all_from_resource")]
        int add_tileset_from_resource(string? location, string name);
        [CCode (cname = "get_tileset_count")]
        uint8 get_tileset_count();
        [CCode (cname = "get_tileset_name")]
        unowned string? get_tileset_name(uint8 id);
        [CCode (cname = "get_tileset_location")]
        unowned string? get_tileset_location(uint8 id);

        [CCode (cname = "draw_maps")]
        void draw(Mat4 mat);
    }

    [CCode (cheader_filename = "settings.h")]
    namespace Settings {
        [CCode (cname = "init_settings")]
        void init();
        [CCode (cname = "cleanup_settings")]
        void cleanup();
    }

    [CCode (cheader_filename = "player.h")]
    namespace Player {
        [CCode (cname = "init_player")]
        void init(uint x, uint y);

        [CCode (cname = "update_player")]
        void update(float delta);

        [CCode (cname = "draw_player")]
        void draw();
    }

    [CCode (cheader_filename = "ui.h")]
    namespace UI {
        [CCode (cname = "init_ui")]
        bool init();

        [CCode (cname = "update_ui")]
        void update(float delta);

        [CCode (cname = "draw_ui")]
        void draw();
    }

    [CCode (cheader_filename = "actor-manager.h")]
    namespace Actors {
        [CCode (cname = "update_actors")]
        void update(float dt);
    }
}
}
