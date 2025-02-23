/**
 * @file driver.c
 * @author hackathon team
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

#define GB4 4294967296
#define BOOK_SIZE 8

/* This allows us to keep memory state private to the driver module */
static Mem_T *mem_state = NULL;

void *convert_address(Mem_T *mem, uint32_t addr);

Mem_T* init_memory_system(uint32_t kernel_size)
{    
    /* Safely initialize the memory state */
    assert(mem_state == NULL);
    mem_state = (Mem_T*) malloc(sizeof(Mem_T));
    assert(mem_state != NULL);

    /* This mmap allocates 4GB of emulated physical memory.
     * Ideally, we would protect this memory by giving the "user" program no 
     * permissions and giving our "kernel" read and write privelege. However,
     * as far as the real OS is concerned, our permission levels are one and the
     * same, so we are enforcing security through abstraction. This is a less
     * than ideal solution and we acknowledge this. We need hardware support to
     * do this properly. */
    void *mem = mmap(NULL, GB4, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    mem_state->mem = mem;
    mem_state->usable_mem = (void*)((char*)mem + BOOK_SIZE);
    mem_state->recycler = recycler_init();

    printf("Start of mem: %p\n", mem_state->mem);
    printf("Start of usable mem: %p\n", mem_state->usable_mem);

    mem_state->kernel_virtual_size = kernel_size;
    mem_state->begin_unused = kernel_size;

    return mem_state;
}

// NOTE: this function is intended to be private to this module. We don't want
// clients accessing 64 bit addresses
inline void *convert_address(Mem_T *mem, uint32_t addr)
{
    void *seg = mem->mem;
    void *ptr = ((char *)seg + addr);
    return ptr;
}

// Liam will do these two functions. They will be much more straightforward
// than the general case virtual allocation

// uint32_t kern_overwrite();

// TODO: Liam
void *kern_malloc(uint32_t size)
{
    (void)size;
    return NULL;
}

// TODO: Liam
void *kern_realloc(uint32_t size, void *addr)
{
    assert(addr == 0);
    (void)size;
    return NULL;
}

uint32_t vs_malloc(Mem_T *mem, uint32_t size)
{
    /* Look for segments to be recycled. If there are freed segments that are
     * ready to be recycled, recycled them */

    uint32_t freed_seg = find_freed_segment(mem, size);

    if (freed_seg != 0) {
        // update capacity, size, and usable beginner address for client
        return freed_seg;
    }

    // If there are no segments to be recycled, carve a fresh one from the heap

    // beginning virtual 
    uint32_t begin_open = mem->begin_unused;
    printf("The start of the unused memory is %u\n", begin_open);
    
    // find number of 32 byte blocks need to support the allocation request
    uint32_t num_blocks = get_blocks_from_alloc_size(size);
    uint32_t capac = 32 * num_blocks;

    // update the beginning of the unused heap
    mem->begin_unused = begin_open + capac;

    // get the beginning of usable physical memory
    uint32_t *phys = (uint32_t*)mem->usable_mem;
    (void)phys;

    char *start_addr = (char*)mem->usable_mem + begin_open;
    (void)start_addr;
    
    // printf("Start addr is %p\n", (void*)start_addr);
    printf("begin_open: %d\n", begin_open);
    phys[begin_open / 4] = capac;
    printf("capacity: %d\n", capac);
    phys[begin_open / 4 + 1] = size;
    printf("size: %d\n", size);


    // Add 8 bytes to the client-facing address (to skip over our bookkeeping)
    return begin_open + 8;
}

uint32_t vs_calloc(Mem_T *mem, uint32_t size){

    /* This will be a wrapper over vs_malloc that calls memset( , 0, ) after
     * allocation. I want to explicitly define this now because I think a
     * segment daemon has the potential to optimize the way that we zero
     * segments. No guarantees, but I don't want the application to worry about
     * this step when we (the engineers) may be able to do it better behind the
     * scenes. Follow the KISS principle: get the interface right, fix the
     * implementation later.
     */

    // do the malloc in the first place
    uint32_t addr = vs_malloc(mem, size);

    // convert the virtual address to physical
    void *ptr = convert_address(mem, addr);

    // memset the physical memory
    memset(ptr, 0, size);

    // return the virtual address
    return addr;
}

// These will call the dangerous ones, which we will eventually want to make
// the program fly

// void safe_set_at

// void safe_get_at


// TODO: these could be made UM specific
void set_at(Mem_T *mem, uint32_t addr, uint32_t size, void *src)
{
    // check bounds
    (void)mem;
    (void)addr;
    (void)size;
    (void)src;
}

void get_at(Mem_T *mem, uint32_t addr, uint32_t size, void *dest)
{
    // check bounds
    (void)mem;
    (void)addr;
    (void)size;
    (void)dest;
}


void vs_free(Mem_T *mem, uint32_t addr)
{
    printf("Freeing the memory segment with id %u\n", addr);

    // Update the free list
    free_segment(mem, addr);
    return;
}

/* 
 * TO BE REVISITED: Interesting architecture problem
 * If we make the segment daemon concurrent, will it be able to zero memory 
 * segments for us before we need them? This could be a really hard problem.
 * We may not get to this on saturday */
// void init_segment_daemon(void)
// {
//     return;
// }
