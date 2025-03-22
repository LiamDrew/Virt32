#ifndef RECYCLER_H
#define RECYCLER_H


#include "mem_state.h"
#include "stack.h"
#include <stdlib.h>

Stack_T *recycler_init(void);

// uint32_t find_freed_segment(uint32_t size);

// void free_segment(uint8_t *umem, uint32_t seg_addr);

// TODO: need a corresponding cleanup function for the recycler

inline uint32_t find_freed_segment(uint32_t size, Stack_T *rec)
{
    uint32_t index = get_idx_from_alloc_size(size);

    assert(index < REC_BUCKETS);

    Stack_T s = rec[index];
    Stack_T *as = &rec[index];

    /* if no segment of size 'size', check next bucket */
    if (stack_is_empty(s))
    {
        return SEG_NOT_FOUND;
    }

    uint32_t freed_segment = stack_pop(as);
    return freed_segment;
}

/* NOTE: Ideally, this function would be concurrent so the UM can keep executing
 * instructions without waiting around for the recycler to update its stacks */
inline void free_segment(uint8_t *umem, uint32_t seg_addr, Stack_T *rec)
{
    uint32_t sys_addr = seg_addr - BOOK_SIZE;

    uint32_t *virt = convert_address(umem, sys_addr);
    uint32_t cap = *virt;

    // printf("The capacity of the freed segment is %u\n", cap);
    int index = ((cap + 8) / 32) - 1;

    /* NOTE: intentionally storing the user-facing v^2 address in the stack
     * for easy future reuse */
    rec[index] = stack_push(rec[index], seg_addr);
}

#endif
