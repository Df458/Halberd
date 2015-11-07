[CCode (cheader_filename = "stack.h", cheader_filename = "render.h")]
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
namespace Editor {
    [CCode (cname="init_render")]
    ushort init_render();
    [CCode (cname="destroy_render")]
    void cleanup_render();
}
}
