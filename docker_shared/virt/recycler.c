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
    // uint32_t bk_addr = seg_addr - 8;

    // /* Grab segment size from bookkeeping */
    // // uint32_t begin_open = mem->begin_unused;
    // uint32_t *phys = (uint32_t*)mem->usable_mem;
    
    // //printf("This is bk_addr %d\n", bk_addr);

    // int seg_cap = phys[65536 + bk_addr];
    
    // //printf("This is seg_cap %d\n", seg_cap);

    // int index = ((seg_cap + 8) / 32) - 1;
    // //printf("index: %d\n", index);
    // stack_push(&((Stack*)(mem->recycler))[index], seg_addr);
    // if (!stack_is_empty(&((Stack*)(mem->recycler))[3])) {
    //     //printf("inserted segment for 128\n");
    // }
}
    uint32_t* phys = convert_address(mem, seg_addr);
    int seg_cap = phys[-2];
    int index = ((seg_cap + 8) / 32) - 1;

    stack_push(&((Stack*)(mem->recycler))[index], seg_addr);
}
