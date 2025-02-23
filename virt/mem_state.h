#ifndef MEM_STATE_H
#define MEM_STATE_H

#include <stdint.h>

#define BOOK_SIZE 8
#define MIN_SEG_SIZE 32

typedef struct Mem_T {
    void *mem;              // pointer to the begining of a 4GB memory segment
    void *usable_mem;       // pointer to the begining of the usable memory
    void *recycler;         // data structure holding the free segment tags/sizes
    uint32_t kernel_virtual_size;   // kernel_size 
    uint32_t begin_unused;
} Mem_T;

uint32_t get_idx_from_alloc_size(uint32_t size);

#endif
