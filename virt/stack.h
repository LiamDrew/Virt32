#ifndef STACK_H
#define STACK_H
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    uint32_t* stack;
    uint32_t size; // Potential remove this and capacity to optimize space
    uint32_t capacity;
} Stack;

Stack* stack_init(uint32_t size);
void stack_push(Stack *s, uint32_t elem);
void stack_expand(Stack* s);
uint32_t stack_pop(Stack* s);
uint32_t stack_top(Stack *s);
void stack_free(Stack* s);
bool stack_is_empty(Stack *s);

#endif 
