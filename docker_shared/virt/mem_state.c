#include "mem_state.h"
#include <stdio.h>

uint32_t get_idx_from_alloc_size(uint32_t size)
{
    // Will allocate 1 block when it gets an exact fit
    uint32_t num_blocks = ((size + BOOK_SIZE - 1) / 32);
    return num_blocks;
}

/* NOTE: currently, speed is more important than safety. We need this function
 * to execute as quickly as possible, and we don't have time for all this
 * dereferencing that's currently happening. */

// NOTE: this function is intended to be private to this module. We don't want
// clients accessing 64 bit addresses
inline void *convert_address(Mem_T *mem, uint32_t addr)
{
    void *seg = mem->usable_mem;
    void *ptr = ((char *)seg + addr);
    return ptr;
}
