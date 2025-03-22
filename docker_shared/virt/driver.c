/**
 * @file driver.c
 * @date February 2025
 * @brief
 * Implement the virtual memory system drivers and interface.
 */

#include "driver.h"
// #include "recycler.h"
// #include <stdlib.h>
// #include <assert.h>
// #include <stdio.h>
// #include <sys/mman.h>
// #include <string.h>


/* The full memory state struct is private to this module. The user only
 * needs the first usable virtual address in order to handle address
 * translations as efficiently as possible (i.e. by passing local variables to
 * the convert address function instead of using global variables) */ 
// static Mem_T *mem = NULL;
// uint8_t *usable = NULL;
// static Stack_T *rec = NULL;

Mem_T *mem = NULL;
uint8_t *usable = NULL;
Stack_T *rec = NULL;
uint32_t start_unused;

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
    rec = recycler_init();
    mem->recycler = rec;


    /* The kernel virtual size is 8 bytes smaller than its physical size */
    mem->kernel_virtual_size = kernel_size - BOOK_SIZE;
    mem->begin_unused = kernel_size;
    start_unused = kernel_size;

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


