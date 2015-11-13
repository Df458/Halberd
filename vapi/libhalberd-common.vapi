[CCode (cheader_filename = "stack.h")]
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
}
