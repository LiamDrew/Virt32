#include "stack.h"
#include <assert.h>

Stack_T* stack_init(uint32_t size)
{
    Stack_T* s = malloc(sizeof(Stack_T));
    s->stack = malloc(size * sizeof(uint32_t));
    s->capacity = size;
    s->size = 0;

    return s;
}

uint32_t stack_top(Stack_T *s)
{
    return s->stack[s->size - 1];
}

void stack_push(Stack_T *s, uint32_t elem)
{
    assert(s != NULL);
    if (s->size == s->capacity) {
        stack_expand(s);
    }

    s->stack[s->size] = elem;
    s->size++;
}

void stack_expand(Stack_T* s)
{
    s->capacity = s->capacity * 2;
    uint32_t* temp = malloc(s->capacity * sizeof(uint32_t));
    for (uint32_t i = 0; i < s->size; i++) {
        temp[i] = s->stack[i];
    }

    free(s->stack);
    s->stack = temp;
}

uint32_t stack_pop(Stack_T* s)
{
    uint32_t top = s->stack[s->size - 1];
    s->size--;
    return top;
}

bool stack_is_empty(Stack_T* s)
{
    return s->size == 0;
}

void stack_free(Stack_T* s)
{
    free(s);
}
