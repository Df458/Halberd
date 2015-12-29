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

    [CCode (cname="ident")]
    const Mat4 Ident;

    [CCode (cheader_filename="io_util.h")]
    namespace IO {
        [CCode (cname="get_base_resource_path")]
        unowned string? get_path();

        [CCode (cname="get_unique_resource_name")]
        string get_unique_name(string? location, string name);

        [CCode (cname="construct_extended_resource_path")]
        string make_path(string? location, string name);

        [CCode (cname="get_extension")]
        unowned string? get_extension(string filename);
    }
}
