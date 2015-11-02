#include "stack.h"
#include "util.h"
#include "stdio.h"
#include "stdlib.h"

// TODO: Write error checking and flesh this out

stack* stack_make()
{
    stack* s = malloc(sizeof(stack));
    s->data = 0;
    s->last_node = s;
    s->next_node = 0;
    return s;
}

void stack_push(stack** s, void* data)
{
    nulltest(s);
    stack* node = malloc(sizeof(stack));
    node->last_node = (*s)->last_node;
    node->next_node = *s;
    node->data = data;
    *s = node;
}

void* stack_pop(stack** s, uint8_t clean)
{
    nulltest(s);
    void* data = (*s)->data;
    stack* temp = *s;
    *s = (*s)->next_node;
    free(temp);
    if(data && clean) {
        fprintf(stderr, "Cleaning stack data...\n");
        free(data);
        data = NULL;
    }
    return data;
}

uint8_t stack_empty(stack* s)
{
    nulltest(s);
    if(s->last_node == s || !s->next_node)
        return 1;
    return 0;
}

void* stack_peek(stack* s)
{
    nulltest(s);
    void* data = s->data;
    return data;
}

void stack_destroy(stack* s)
{
    nulltest(s);
    while(!stack_empty(s)) {
        stack_pop(&s, 1);
    }
    free(s);
}
