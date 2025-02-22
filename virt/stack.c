#include "stack.h"

void stack_init(Stack* s, uint32_t size) 
{
    s->stack = malloc(size * sizeof(Memory_t));
}

void stack_push(Stack *s, Memory_t elem)
{
    
}

void stack_expand(Stack *s)
{
    s->capacity = s->capacity * 2;
    Memory_t* temp = malloc(s->capacity * sizeof(Memory_t));
    for (int i = 0; i < s->size; i++) {
        temp[i] = s->stack;
    }

    
}