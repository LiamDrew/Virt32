#include "recycler.h"
#include <stdlib.h>
#include <stdio.h>

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
    uint32_t index = get_blocks_from_alloc_size(size) - 2;
    printf("This is index %d\n", index);
    Stack* s = &((Stack*)mem->recycler)[index];

    /* if no segment of size 'size', check next power of two */
    if (stack_is_empty(s)) {
        uint32_t new_idx = (index + 1) % SIZE;
        Stack* next_s = &((Stack*)mem->recycler)[new_idx];
        printf("Inside the next index\n");

        if (stack_is_empty(next_s)) {
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
    int index = get_blocks_from_alloc_size(segment) - 1;
    printf("index: %d\n", index);
    stack_push(&((Stack*)(mem->recycler))[index], segment);
}

