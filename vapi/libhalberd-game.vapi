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

    [CCode (cheader_filename = "controls.h")]
    namespace Controls {
        [CCode (cname = "set_up_state")]
        void set_up_state(bool state);
        [CCode (cname = "set_down_state")]
        void set_down_state(bool state);
        [CCode (cname = "set_left_state")]
        void set_left_state(bool state);
        [CCode (cname = "set_right_state")]
        void set_right_state(bool state);
        [CCode (cname = "set_action_state")]
        void set_action_state(bool state);
        [CCode (cname = "set_confirm_state")]
        void set_confirm_state(bool state);
        [CCode (cname = "set_cancel_state")]
        void set_cancel_state(bool state);
        [CCode (cname = "set_menu_state")]
        void set_menu_state(bool state);
        [CCode (cname = "set_quit_state")]
        void set_quit_state(bool state);
        [CCode (cname = "update_input_states")]
        void update();
    }

    [CCode (cheader_filename = "actor-manager.h")]
    namespace Actors {
        [CCode (cname = "update_actors")]
        void update(float dt);
    }
}
}
