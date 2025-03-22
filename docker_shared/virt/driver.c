/**
 * @file driver.c
 * @date February 2025
 * @brief
 * Implement the virtual memory system drivers and interface.
 */

#include "driver.h"
#include "recycler.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

#define GB4 ((uint64_t)1 << 32)     /* 4 GB = 2^32 */
#define BOOK_SIZE 8
#define BLOCK_SIZE 32

/* The full memory state struct is private to this module. The user only
 * needs the first usable virtual address in order to handle address
 * translations as efficiently as possible (i.e. by passing local variables to
 * the convert address function instead of using global variables) */ 
static Mem_T *mem = NULL;
uint8_t *usable = NULL;

uint8_t *init_memory_system(uint32_t kernel_size)
{    
    /* Safely initialize the memory state */
    assert(mem == NULL);
    mem = (Mem_T*) malloc(sizeof(Mem_T));
    assert(mem != NULL);

    /* Allocate 4 GB of contiguous virtual memory */
    void *virt = mmap(NULL, GB4, PROT_READ | PROT_WRITE, 
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    usable = ((uint8_t*)virt + BOOK_SIZE);
    
    mem->mem = virt;
    mem->usable_mem = (void*)((uint8_t*)virt + BOOK_SIZE);
    mem->recycler = recycler_init();

    /* The kernel virtual size is 8 bytes smaller than its physical size */
    mem->kernel_virtual_size = kernel_size - BOOK_SIZE;
    mem->begin_unused = kernel_size;

    return usable;
}

void terminate_memory_system(void)
{
    /* Free the memory object statically defined within this file */
    munmap(mem->mem, GB4);
    // TODO: also need to free the recycler
}

uint32_t kern_realloc(uint32_t size)
{
    /* Ensure the kernel has enough space to fill the allocation */
    assert(mem->kernel_virtual_size >= size);

    /* Update the first 8 bytes of virtual memory with kernel bookkeeping */
    uint32_t *mem_start = (uint32_t*)mem->mem;
    *mem_start = mem->kernel_virtual_size;
    mem_start++;
    *mem_start = size;

    /* The base kernel virtual address is always 0. */
    return 0; 
}


void kern_memcpy(uint32_t src_addr, uint32_t copy_size)
{
    /* This function will overwrite the kernel memory (segment 0). The user
     * does not control the destination this memory is copied to; the kernel
     * does. */

    /* Get real source and destination addresses to use with memcpy */
    uint8_t *umem = usable;
    void *real_src = convert_address(umem, src_addr);
    void *real_dest = convert_address(umem, 0);
    memcpy(real_dest, real_src, copy_size);
    return;
}

uint32_t vs_malloc(uint32_t size)
{
    /* Users may only ask vs_malloc for (2^24 - 8) bytes of contiguous space */
    // printf("Size is %u\n", size);
    assert(size < MAX_ALLOC);

    /* Look for segments to be recycled. If there are freed segments that are
     * ready to be recycled, recycled them */
    uint32_t freed_seg = find_freed_segment(size);

    /* check that a free segment is available */
    if (freed_seg != SEG_NOT_FOUND) {
        uint32_t *freed_seg_addr = convert_address(usable, freed_seg);
        freed_seg_addr[-1] = size;
        return freed_seg;
    }
    
    /* If no segments can be recycled, carve a fresh one from the heap */
    
    // beginning virtual
    uint32_t user_start = mem->begin_unused + BOOK_SIZE;
    // printf("The start of the unused memory is %u\n", begin_open);
    
    /* Adding 1 to the idx to get the number of blocks the alloc needs
     * find number of 32 byte blocks need to support the allocation request */
    uint32_t num_blocks = get_idx_from_alloc_size(size) + 1;
    uint32_t user_cap = (num_blocks * BLOCK_SIZE) - BOOK_SIZE;
    
    /* Check that we still have enough 'carvable' memory in the 4GB segmment */
    /* add 8 to user start to account for initial bookkeeping, needed for GB4 */
    assert(GB4 - (user_start + BOOK_SIZE) >= user_cap);

    // update the beginning of the unused heap
    mem->begin_unused = user_start + user_cap;

    uint32_t *user_addr = convert_address(usable, user_start);

    user_addr[-2] = user_cap;
    user_addr[-1] = size;

    return user_start;
}

uint32_t vs_calloc(uint32_t size)
{
    /* Make a v^2 allocation */
    uint32_t addr = vs_malloc(size);
    
    /* Convert the v^2 address to a virtual address */
    void *ptr = convert_address(usable, addr);
    (void)ptr;

    /* NOTE: zeroing out the memory is only necessary when recycling a segment
     * MAP_ANONYMOUS guarantees that all carved memory will be 0 */

    /* set the virtual memory to zero */
    memset(ptr, 0, size);

    /* Return the virtual address */
    return addr;
}

void vs_free(uint32_t addr)
{
    // printf("Freeing the memory segment with id %u\n", addr);

    // Update the free list
    free_segment(usable, addr);
    return;
}

/* TODO: these two functions need thorough cleanup and testing. This can wait
 * because they are not useful for the performant VM, but would be useful in an
 * application where memory safety is more important that speed. */

// void safe_set_at(Mem_T *mem, uint32_t base, uint32_t offset, uint32_t value)
// {
//     // check base is a valid base
//     assert(!(base % 32));

//     // bounds checking: ensure user accesses memory they have permissions on
//     uint32_t spot_to_access = base + offset;
//     // uint32_t *ptr_to_spot = convert_address(base, offset);
//     uint32_t *ptr_to_spot = convert_address(mem, spot_to_access);
//     uint32_t size = ptr_to_spot[-(offset + 1)];
//     assert(offset < size);
    
//     // call set at
//     set_at(mem, base + offset, value);
// }

// uint32_t safe_get_at(Mem_T *mem, uint32_t base, uint32_t offset)
// {
//     // check base is a valid base
//     assert(!(base % 32));

//     // bounds checking: ensure user accesses memory they have permissions on
//     uint32_t spot_to_access = base + offset;
//     // uint32_t *ptr_to_spot = convert_address(base, offset);
//     uint32_t *ptr_to_spot = convert_address(mem, spot_to_access);
//     uint32_t size = ptr_to_spot[-(offset + 1)];
//     assert(offset < size);

//     // call get at
//     return get_at(mem, base + offset);
// }


