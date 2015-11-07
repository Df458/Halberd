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
}
}
