#include "recycler.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define SIZE 32769 /* User can allocate at most 1GB */

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
    uint32_t index = get_idx_from_alloc_size(size);
    assert(index < SIZE);
    
    Stack* s = &((Stack*)mem->recycler)[index];

    /* if no segment of size 'size', check next bucket */
    if (stack_is_empty(s)) {
        return 0;
    }
    
    uint32_t freed_segment = stack_pop(s);
    return freed_segment;
}

/* Assuming that the seg_addr received is already processed */
void free_segment(Mem_T *mem, uint32_t seg_addr)
{
    uint32_t* phys = convert_address(mem, seg_addr);
    int seg_cap = phys[-2];
    int index = ((seg_cap + 8) / 32) - 1;

    stack_push(&((Stack*)(mem->recycler))[index], seg_addr);
}
