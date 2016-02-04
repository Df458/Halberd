#include "stack.h"
#include "dfgame-common.h"
#include "stdio.h"
#include "stdlib.h"

// TODO: Write error checking and flesh this out

stack* stack_make()
{
    stack* s = malloc(sizeof(stack));
    s->top_node = 0;
    s->bottom_node = 0;
    return s;
}

void stack_push(stack* s, void* data)
{
    nulltest(s);
    struct stack_node* node = malloc(sizeof(struct stack_node));
    node->next_node = s->top_node;
    s->top_node = node;
    if(!s->bottom_node)
        s->bottom_node = node;
    node->data = data;
}

void* stack_pop(stack* s, uint8_t clean)
{
    nulltest(s);
    if(stack_empty(s))
        return 0;
    void* data = s->top_node->data;
    struct stack_node* temp = s->top_node;
    if(s->bottom_node == s->top_node)
        s->bottom_node = 0;
    s->top_node = s->top_node->next_node;
    free(temp);
    if(data && clean) {
        /*fprintf(stderr, "Cleaning stack data...\n");*/
        free(data);
        data = NULL;
    }
    return data;
}

uint8_t stack_empty(stack* s)
{
    nulltest(s);
    if(!s->bottom_node)
        return 1;
    return 0;
}

void* stack_peek(stack* s)
{
    nulltest(s);
    void* data = 0;
    if(s->top_node)
        data = s->top_node->data;
    return data;
}

void stack_destroy(stack* s)
{
    nulltest(s);
    while(!stack_empty(s)) {
        stack_pop(s, 1);
    }
    free(s);
}
