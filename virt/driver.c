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

void init_segment_daemon(void);

void init_memory_system(uint32_t kernel_size)
{
    (void)kernel_size;
    return;
}

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

void *vs_malloc(uint32_t size)
{
    // Look for segments to be recycled. If there are freed segments that are
    // ready to be recycled, recycled them
    void *temp = find_freed_segment();
    (void)temp;

    // If there are no segments to be recycled, carve a fresh one from the deep
    // blue C

    (void)size;
    return NULL;
}

void *vs_calloc(uint32_t size){

    /* This will be a wrapper over vs_malloc that calls memset( , 0, ) after
     * allocation. I want to explicitly define this now because I think a
     * segment daemon has the potential to optimize the way that we zero
     * segments. No guarantees, but I don't want the application to worry about
     * this step when we (the engineers) may be able to do it better behind the
     * scenes. Follow the KISS principle: get the interface right, fix the
     * implementation later.
     */

    (void)size;
    return NULL;
}

void vs_free(void *addr)
{
    (void)addr;
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
