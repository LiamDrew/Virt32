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


//TODO: revert to beg_unused if no stack opt. exists
uint32_t find_freed_segment(Mem_T *mem, uint32_t size)
{
    printf("Size here is %u\n", size);
    uint32_t index = get_idx_from_alloc_size(size);
    printf("This is index %d\n", index);
    Stack* s = &((Stack*)mem->recycler)[index];

    /* if no segment of size 'size', check next power of two */
    if (stack_is_empty(s)) {
        // uint32_t new_idx = (index + 1) % SIZE;
        // Stack* next_s = &((Stack*)mem->recycler)[new_idx];
        // // printf("Inside the next index\n");

        // // TODO: Add the functionality of splitting the next segment 
        // if (stack_is_empty(next_s)) {
        //     return 0;
        // }

        // uint32_t segment = stack_pop(next_s);
        // uint32_t *phys = (uint32_t*)mem->usable_mem;
        // uint32_t bk_addr = phys[segment - 8];
        // int seg_cap = phys[65536 + bk_addr];
        // int seg_size = phys[65536 + bk_addr + 1];
        // int new_cap = ((seg_cap + 8) / 2) - 8;

        // phys[65536 + bk_addr] = 

        return 0;
    }
    
    uint32_t freed_segment = stack_pop(s);
    return freed_segment;
}

// Assuming that the seg_addr received is already processed
void free_segment(Mem_T *mem, uint32_t seg_addr)
{
    uint32_t bk_addr = seg_addr - 8;

    /* Grab segment size from bookkeeping */
    // uint32_t begin_open = mem->begin_unused;
    uint32_t *phys = (uint32_t*)mem->usable_mem;
    
    printf("This is bk_addr %d\n", bk_addr);

    int seg_cap = phys[65536 + bk_addr];
    
    printf("This is seg_cap %d\n", seg_cap);

    int index = ((seg_cap + 8) / 32);
    printf("index: %d\n", index);
    stack_push(&((Stack*)(mem->recycler))[index], seg_addr);
    if (!stack_is_empty(&((Stack*)(mem->recycler))[3])) {
        printf("inserted segment for 128\n");
    }
}

