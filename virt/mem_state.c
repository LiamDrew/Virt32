#include "mem_state.h"
#include <stdio.h>

uint32_t get_blocks_from_alloc_size(uint32_t size)
{
    // Will allocate 1 block when it gets an exact fit
    uint32_t num_blocks = ((size + BOOK_SIZE - 1) / 32);
    return num_blocks;
}

