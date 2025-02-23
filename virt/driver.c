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

void *convert_address(Mem_T *mem, uint32_t addr);

void init_segment_daemon(void);

Mem_T* init_memory_system(uint32_t kernel_size)
{
    (void)kernel_size;
    
    Mem_T *mem_state = (Mem_T*) malloc(sizeof(Mem_T));
    assert(mem_state != NULL);

    // printf("X is %lu\n", GB4);

    // User gets no permission until they do anything
    void *mem = mmap(NULL, GB4, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // void *mem = mmap(NULL, GB4, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    mem_state->mem = mem;

    // TODO: will have to init the recycler
    mem_state->recycler = NULL;

    mem_state->kernel_size = kernel_size;
    mem_state->beg_unused = kernel_size;

    return mem_state;
}

// Liam will do these two functions. They will be much more straightforward
// than the general case virtual allocation
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
    // Look for segments to be recycled. If there are freed segments that are
    // ready to be recycled, recycled them
    // uint32_t temp = find_freed_segment(mem, size);

    // if (temp != NULL) {
        // update capacity, size, and usable beginner address for client
        // return temp
    // }

    // If there are no segments to be recycled, carve a fresh one from the heap

    // The starting index beyond which the heap is large
    // TODO: get this from kernel size on init
    // Keep track of this as program runs

    uint32_t beg_open = mem->beg_unused;
    printf("The start of the unused memory is %u\n", beg_open);
    
    // find number of 32 byte blocks need to support the allocation request
    uint32_t num_blocks = ((size + 8 - 1) / 32) + 1;
    uint32_t capac = 32 * num_blocks;

    // update the beginning of the unused heap
    mem->beg_unused = beg_open + capac;

    // get the physical memory
    uint32_t *phys = (uint32_t*)mem->mem;
    (void)phys;

    char *start_addr = (char*)mem->mem + beg_open;
    
    printf("Start addr is %s\n", start_addr);

    phys[beg_open] = capac;
    phys[beg_open + 1] = size;

    // Add 8 bytes to the client-facing address (to skip over our bookkeeping)
    return beg_open + 8;
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


// NOTE: this function is intended to be private to this module. We don't want
// clients accessing 64 bit addresses
void *convert_address(Mem_T *mem, uint32_t addr)
{
    void *seg = mem->mem;
    void *ptr = ((char *)seg + addr);
    return ptr;
}

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
    // have to update the free list 
    printf("Freeing the memory segment with id %u\n", addr);
    (void)mem;
    (void)addr;

    // TODO: put this back in
    // free_segment(mem, addr);
    return;
}

/* 
 * TO BE REVISITED: Interesting architecture problem
 * If we make the segment daemon concurrent, will it be able to zero memory 
 * segments for us before we need them? This could be a really hard problem.
 * We may not get to this on saturday */
void init_segment_daemon(void)
{
    return;
}
