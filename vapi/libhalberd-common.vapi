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
}
