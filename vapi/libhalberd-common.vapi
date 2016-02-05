[CCode (cheader_filename = "stack.h", cheader_filename = "matrix.h")]
namespace Halberd {
    [CCode (cname="stack", free_function="stack_destroy")]
    [Compact]
    public class Stack {
        [CCode (cname="stack_make")]
        public Stack();

        [CCode (cname="stack_push")]
        public void push(void* data);

        [CCode (cname="stack_pop")]
        public void* pop(ushort clean);
    }

    [CCode (cname="mat4", destroy_function="", default_value="ident", has_type_id=false)]
    [SimpleType]
    public struct Mat4 {
        float data[16];
    }

    [CCode (cname = "int", cprefix = "FLAG_", has_type_id = false)]
    [Flags]
    public enum ActorFlags {
        SOLID,
        VISIBLE,
        LOCK_TO_GRID,
        CAN_ORIENT,
        BLOCK_WITH_SOLID
    }

    [CCode (cname="struct actordata", destroy_function="")]
    public struct ActorData {
        uint32 position_x;
        uint32 position_y;
        int16  grid_x;
        int16  grid_y;
        int16  super_grid_x;
        int16  super_grid_y;
        uint16 speed;
        uint8  moving;
        uint8  orientation;
        uint8  flags;
        int8   animation_index;
        uint8  animation_playing;
        float  animation_timer;
        uint32 sprites_id;
    }

    [CCode (cname="struct Actor", destroy_function="")]
    [SimpleType]
    public struct Actor {
        ActorData data;
    }

    [CCode (cname="animation", destroy_function = "")]
    public struct Animation {
        uint8  orients;
        uint16 dimensions_x;
        uint16 dimensions_y;
        int16  origin_x;
        int16  origin_y;

        int8  length;
        uint8 delay;
        bool  loop;
        bool  play;
        
        string handle;
    }

    [CCode (cheader_filename="texture_util.h", cname="sprite", destroy_function="")]
    public struct Sprite{
        Animation[] animations;
        uint8 animation_count;
    }

    [CCode (cheader_filename="editor_io.h", cname="create_blank_actor_for_resource")]
    public unowned Actor? create_actor_for_resource(string? path, string name);

    [CCode (cheader_filename="editor_io.h", cname="save_actor")]
    public void save_actor();

    [CCode (cheader_filename="editor_io.h", cname="register_actor")]
    public void register_actor(Actor? a, string? path, string name);

    [CCode (cname="load_actor_from_resource")]
    public unowned Actor? load_actor_from_resource(string? path, string name);

    [CCode (cname="ident")]
    const Mat4 Ident;

    [CCode (cheader_filename="io_util.h", cheader_filename="asset_registry.h")]
    namespace IO {
        [CCode (cname="get_base_resource_path")]
        unowned string? get_path();

        [CCode (cname="get_unique_resource_name")]
        string get_unique_name(string? location, string name);

        [CCode (cname="construct_extended_resource_path")]
        string make_path(string? location, string name);

        [CCode (cname="get_extension")]
        unowned string? get_extension(string filename);

        [CCode (cname="resource_loader", has_target=false)]
        public delegate void* resource_loader(string? location, string name);

        [CCode (cname="resources_init")]
        public bool init(resource_loader loader);
        [CCode (cname="resources_cleanup")]
        public void cleanup();

        [CCode (cname="load_anonymous_resource")]
        public void* load(string? location, string name);

        [CCode (cname="get_id_from_resource")]
        public uint32 get_id_from_resource(string? location, string name, bool should_create);

        [CCode (cname="get_name_from_id")]
        public string? get_name_from_id(uint32 id, bool force_invalid = false);

        [CCode (cname="get_path_from_id")]
        public string? get_path_from_id(uint32 id, bool force_invalid = false);

        [CCode (cname="move_resource")]
        public void move_resource(string? prev_location, string prev_name, string? next_location, string next_name);

        [CCode (cname="delete_resource")]
        public void delete_resource(string? resource_location, string resource_name);

        [CCode (cname="move_id")]
        public void move_id(uint32 id, string? next_location, string next_name);

        [CCode (cname="delete_id")]
        public void delete_id(uint32 id);

        [CCode (cname="get_failed_count")]
        public int32 get_failed_count();

        [CCode (cname="get_failed_ids")]
        public int32[] get_failed_ids();
    }
}
