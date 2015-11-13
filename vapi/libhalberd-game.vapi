[CCode (cheader_filename = "actor.h")]
namespace Halberd {
namespace Game {
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
        [CCode (cname = "load_maps")]
        void load(string filepath);
        [CCode (cname = "save_maps")]
        void save();
    }

    [CCode (cheader_filename = "settings.h")]
    namespace Settings {
        [CCode (cname = "init_settings")]
        void init();
        [CCode (cname = "cleanup_settings")]
        void cleanup();
    }
}
}
