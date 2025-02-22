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

#define GB4 4294967296

void init_segment_daemon(void);

Mem_T* init_memory_system(uint32_t kernel_size)
{
    (void)kernel_size;
    
    Mem_T *mem_state = (Mem_T*) malloc(sizeof(Mem_T));
    assert(mem_state != NULL);

    printf("X is %lu\n", GB4);

    void *mem = mmap(NULL, GB4, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    mem_state->mem = mem;
    // TODO: will have to init the recycler
    mem_state->recycler = NULL;

    return mem_state;
}

// Liam will do these two functions. They will be much more straightforward
// than the general kernel allocation
void *kern_malloc(uint32_t size)
{
    (void)size;
    return NULL;
}

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
    // (void)temp;

    // If there are no segments to be recycled, carve a fresh one from the deep
    // blue C

    (void)mem;
    (void)size;

    return 0;
}

// void *vs_calloc(uint32_t size){

//     /* This will be a wrapper over vs_malloc that calls memset( , 0, ) after
//      * allocation. I want to explicitly define this now because I think a
//      * segment daemon has the potential to optimize the way that we zero
//      * segments. No guarantees, but I don't want the application to worry about
//      * this step when we (the engineers) may be able to do it better behind the
//      * scenes. Follow the KISS principle: get the interface right, fix the
//      * implementation later.
//      */

//     (void)size;
//     return NULL;
// }

void vs_free(Mem_T *mem, uint32_t addr)
{
    // have to update the free list 
    free_segment(mem, addr);
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
