#include "recycler.h"
#include <stdlib.h>

#define SIZE 27

void* recycler_init(void)
{
    void* recycler = malloc(sizeof(Stack) * SIZE);
    for (int i = 0; i < SIZE; i++) {
        Stack* s = stack_init(10);
        ((Stack*)recycler)[i] = *s;
    }

    return recycler;
}

uint32_t find_freed_segment(Mem_T *mem, uint32_t size)
{
    int index = size; /* TODO: get log2 of size */
    Stack* s = &((Stack*)mem->recycler)[index];

    if (stack_is_empty(s) == -1) {
        int new_idx = (index + 1) % SIZE;
        Stack* next_s = &((Stack*)mem->recycler)[new_idx];

        if (stack_is_empty(next_s) == -1) {
            return -1;
        }

        return stack_pop(next_s);
    }
    
    uint32_t freed_segment = stack_pop(s);
    return freed_segment;
}

// Assuming that the seg_addr received is already processed
void free_segment(Mem_T *mem, uint32_t seg_addr)
{
    uint32_t segment = seg_addr - 8;
    stack_push(&((Stack*)(mem->recycler))[seg_addr], segment);
    return;
}

