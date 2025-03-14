#ifndef STACK_H
#define STACK_H
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t* stack;
    uint32_t size; // Potential remove this and capacity to optimize space
    uint32_t capacity;
} Stack_T;

Stack_T* stack_init(uint32_t size);
void stack_push(Stack_T *s, uint32_t elem);
void stack_expand(Stack_T* s);
uint32_t stack_pop(Stack_T* s);
uint32_t stack_top(Stack_T *s);
void stack_free(Stack_T* s);
bool stack_is_empty(Stack_T *s);



#endif 
