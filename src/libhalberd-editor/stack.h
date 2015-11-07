#ifndef STACK_H
#define STACK_H
#include <inttypes.h>

struct stack_node
{
    struct stack_node* next_node;
    void* data;
};

typedef struct stack
{
    struct stack_node* top_node;
    struct stack_node* bottom_node;
} stack;

stack* stack_make();
void stack_push(stack* stack, void* data);
void* stack_pop(stack* stack, uint8_t clean);
uint8_t stack_empty(stack* stack);
void* stack_peek(stack* stack);
void stack_destroy(stack* stack);

#endif
