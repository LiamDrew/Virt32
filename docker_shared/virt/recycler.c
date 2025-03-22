#include "recycler.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define INIT_STACK_SIZE 2

static Stack_T *rec = NULL;

/* TODO: transition this to keeping the recycler private to this module. There
 * should be no need to return the pointer to the Stacks */
Stack_T *recycler_init(void)
{
    assert(rec == NULL);
    rec = malloc(sizeof(Stack_T) * REC_BUCKETS);
    assert(rec != NULL);

    Stack_T *recycler = rec;

    for (uint32_t i = 0; i < REC_BUCKETS; i++)
    {
        Stack_T *s = stack_init(INIT_STACK_SIZE);
        recycler[i] = *s;
    }

    return recycler;
}

uint32_t find_freed_segment(uint32_t size)
{
    uint32_t index = get_idx_from_alloc_size(size);
    
    /* Debugging for now */
    // printf("\n\n Finding freed segment: Index is %u, size is %u\n\n", index, size);
    // assert(false);

    assert(index < REC_BUCKETS);
    
    // Stack_T* s = &((Stack_T*)mem->recycler)[index];
    Stack_T *s = &rec[index];

    /* if no segment of size 'size', check next bucket */
    if (stack_is_empty(s)) {
        return SEG_NOT_FOUND;
    }
    
    uint32_t freed_segment = stack_pop(s);
    return freed_segment;
}

/* NOTE: Ideally, this function would be concurrent so the UM can keep executing
 * instructions without waiting around for the recycler to update its stacks */
void free_segment(uint8_t *umem, uint32_t seg_addr)
{
    uint32_t sys_addr = seg_addr - BOOK_SIZE;

    uint32_t *virt = convert_address(umem, sys_addr);
    uint32_t cap = *virt;

    // printf("The capacity of the freed segment is %u\n", cap);
    int index = ((cap + 8) / 32) - 1;

    /* NOTE: intentionally storing the user-facing v^2 address in the stack
     * for easy future reuse */
    stack_push(&rec[index], seg_addr);
}
