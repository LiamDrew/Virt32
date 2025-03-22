/**
 * @file driver.h
 * @date February 2025
 * @brief
 * The interface for initializing and using the virtual memory system.
 */

#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include "mem_state.h"
#include "recycler.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

extern uint8_t *usable;
extern Stack_T *rec;
extern Mem_T *mem;
extern uint32_t start_unused;

// Moving this function to the mem_state module;
// void *convert_address(Mem_T *mem, uint32_t addr);

/* Initialize the virtual memory system */
uint8_t *init_memory_system(uint32_t kernel_size); //checked, 2 mem_state vars on lookout

void terminate_memory_system(void);

/* Kernel Allocator (kern_remalloc):
 * Overwrite whatever is in the zero segment and initialize all requested
 * memory to zero. In a real OS, the kernel memory management needs to be
 * significantly more complicated, but since we are designing this memory system
 * for use in a very simple virtual machine. */
uint32_t kern_realloc(uint32_t size);

/* Kernel Memory Copy (kern_memcpy):
 * Provides users of the memory system with an interface to copy data in and
 * out of kernel space. */
void kern_memcpy(uint32_t src_addr, uint32_t copy_size);

/* Virtual Segment Malloc (vs_malloc): 
 * Carve out a segment of physical memory 
 * and serve it to the program as virtual memory. */
// uint32_t vs_malloc(uint32_t size);

/* Virtual segment calloc (vs_calloc): Same thing as vs_malloc, but zero all
 * the memory needed ahead of time */
// uint32_t vs_calloc(uint32_t size);

inline uint32_t vs_calloc(uint32_t size)
{
    /* Users may only ask vs_malloc for (2^24 - 8) bytes of contiguous space */
    assert(size < MAX_ALLOC);

    /* Look for segments to be recycled. If there are freed segments that are
     * ready to be recycled, recycled them */
    uint32_t freed_seg = find_freed_segment(size, rec);

    /* check that a free segment is available */
    if (freed_seg != SEG_NOT_FOUND)
    {
        uint32_t *freed_seg_addr = convert_address(usable, freed_seg);
        freed_seg_addr[-1] = size;

        memset(freed_seg_addr, 0, size);

        return freed_seg;
    }

    /* If no segments can be recycled, carve a fresh one from the heap */
    uint32_t user_start = start_unused + BOOK_SIZE;

    /* Find the number of 32 byte blocks need to fill the allocation */
    uint32_t num_blocks = get_idx_from_alloc_size(size) + 1;
    uint32_t user_cap = (num_blocks * BLOCK_SIZE) - BOOK_SIZE;

    /* Check that we still have enough 'carvable' memory in the heap.
     * Add BOOK_SIZE to user start to account for kernel bookkeeping */
    assert(GB4 - (user_start + BOOK_SIZE) >= user_cap);

    /* Update the beginning of the unused heap */
    start_unused = user_start + user_cap;

    uint32_t *user_addr = convert_address(usable, user_start);

    user_addr[-2] = user_cap;
    user_addr[-1] = size;

    return user_start;
}

/* Virtual Segment Free (vs_free):
 * Free a virtual segment for future use. */
inline void vs_free(uint32_t addr)
{
    free_segment(usable, addr, rec);
}

inline void set_at(uint8_t *umem, uint32_t addr, uint32_t value)
{
    uint32_t *dest = convert_address(umem, addr);
    *dest = value;
}

inline uint32_t get_at(uint8_t *umem, uint32_t addr)
{
    uint32_t *src = convert_address(umem, addr);
    return *src;
}

// void safe_set_at(Mem_T *mem, uint32_t base, uint32_t offset, uint32_t value);

// uint32_t safe_get_at(Mem_T *mem, uint32_t base, uint32_t offset);

#endif
