/**
 * @file driver.c
 * @date February 2025
 * @brief
 * Implement the virtual memory system drivers and interface.
 */

////////////////////////////////////////
/* bad practice. All of our includes should go in the .h, unless we need to use
 * some of them 'secretly' (which is fine, it needs to be purposeful, though)*/
#include "driver.h"
#include "recycler.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

// #define GB4 4294967296
#define GB4 ((uint64_t)1 << 32)
#define BOOK_SIZE 8
#define BLOCK_SIZE 32

/* This allows us to keep memory state private to the driver module */ 
////////////////////////////////////////
/* I disagree. This allows us to keep a specific instance of a memory state
 * struct private to the driver file, in theory. In practice, we end up 
 * returning this specific struct to the user. This, alongside mem_state.h 
 * containing the struct actually makes this suuuuuper public. */
static Mem_T *mem_state = NULL;

/* Convert address is private to this module */
// void *convert_address(Mem_T *mem, uint32_t addr);

Mem_T* init_memory_system(uint32_t kernel_size)
{    
    /* Safely initialize the memory state */
    assert(mem_state == NULL);
    mem_state = (Mem_T*) malloc(sizeof(Mem_T));
    assert(mem_state);

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

    // printf("Start of mem: %p\n", mem_state->mem);
    // printf("Start of usable mem: %p\n", mem_state->usable_mem);

    // The kernel virtual size should be 8 bytes smaller than the physical size
    // 
    mem_state->kernel_virtual_size = kernel_size - 8;
    mem_state->begin_unused = kernel_size;

    return mem_state;
}

void terminate_memory_system(Mem_T *mem)
{
    // Free the huge page
    munmap(mem->mem, GB4);

    // TODO: also need to free the recycler
}

uint32_t kern_recalloc(Mem_T *mem, uint32_t size)
{
    //printf("Kernel Recalloc\n");
    //printf("User requesting %u bytes. There are %u bytes available\n", size, mem->kernel_virtual_size);

    // Assert that the kernel has enough protected space for us to use
    assert(mem->kernel_virtual_size >= size);

    // Update the first 8 bytes of "physical" memory with kernel bookkeeping
    uint32_t *mem_start = (uint32_t*)mem->mem;
    *mem_start = mem->kernel_virtual_size;
    mem_start++;
    *mem_start = size;  // allow the kernel to use only the memory it requested

    // zero out all the bytes the user want
    memset(mem->usable_mem, 0, size);

    // printf("Successfully allocated kernel memory\n");

    return 0;
}


void kern_memcpy(Mem_T *mem, uint32_t src_addr, uint32_t dest_addr, uint32_t copy_size)
{
    // only allow memcpy from user space to kernel space
    // TODO: build some safety into this

    assert(dest_addr == 0);
    
    // get real source and dest addresses
    void *real_src = convert_address(mem, src_addr);
    void *real_dest = convert_address(mem, dest_addr);

    printf("real_src is %p: real_src\n", real_src);
    printf("real_src is %p: real_dest\n", real_dest);
    printf("Copy size is %u\n", copy_size);

    // memcpy(real_dest, real_src, copy_size);
}

uint32_t vs_malloc(Mem_T *mem, uint32_t size)
{
    /* users may only ask vs_malloc for 1MB of contiguous space, maximum */
    //assert(((size >> 32) - 8) < (2 << 30))
    /* Look for segments to be recycled. If there are freed segments that are
     * ready to be recycled, recycled them */
    
    uint32_t freed_seg = find_freed_segment(mem, size);

     /* check that a free segment is available */
    if (freed_seg) {
        // update capacity, size, and usable beginner address for client
        uint32_t *freed_seg_addr = convert_address(mem, freed_seg);
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

    uint32_t *user_addr = convert_address(mem, user_start);
    
    // printf("Start addr is %p\n", (void*)start_addr);

    user_addr[-2] = user_cap;
    user_addr[-1] = size;

    // printf("This is start addr: %d\n", user_start);

    // uint32_t *phys = convert_address(mem, user_start);
    
    // printf("This is bk_addr %d\n", bk_addr);  
    // int seg_cap = phys[-2];

    // printf("This is seg_cap after init %d\n", seg_cap);

    // int seg_cap = phys[65536];
    
    // printf("This is seg_cap right after init %d\n", seg_cap);

    // Add 8 bytes to the client-facing address (to skip over our bookkeeping)
    return user_start;
}

// // NOTE: this function is intended to be private to this module. We don't want
// // clients accessing 64 bit addresses
// inline void *convert_address(Mem_T *mem, uint32_t addr)
// {
//     void *seg = mem->usable_mem;
//     void *ptr = ((char *)seg + addr);

//     // printf("Address %u has been converted to pointer %p\n", addr, ptr);
//     return ptr;
// }

uint32_t vs_calloc(Mem_T *mem, uint32_t size){

    /* This will be a wrapper over vs_malloc that calls memset( , 0, ) after
     * allocation. I want to explicitly define this now because I think a
     * segment daemon has the potential to optimize the way that we zero
     * segments. No guarantees, but I don't want the application to worry about
     * this step when we (the engineers) may be able to do it better behind the
     * scenes. Follow the KISS principle: get the interface right, fix the
     * implementation later.
     * 
     * There is certainly a more efficient way to do this. We will optimize
     * this later.
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

/*
 * Important design choice: since we are pretending to be the "kernel", but we
 * don't have the privilege of the real kernel, we are somewhat limited in
 * our ability to provide a really nice intuitive interface to our memory system
 * One thing we considered was using memcpy to allow the client to set an
 * arbitrary amout of memory (with bounds checking), but that requires a system
 * call to memcpy for every single memory access, which is unacceptably slow.
 *
 * Since this is a hackathon, we are making the restricting assumption that all
 * memory accesses will be 4 bytes so that our system can be used in the UM.
 * A complete virtual memory system would have to provide a better interface
 * than this, but it will do for our purposes. */
// void set_at(Mem_T *mem, uint32_t base, uint32_t offset, uint32_t value)
// {
//     // convert v^2 address to virtual address
//     uint32_t *dest = (uint32_t*) convert_address(mem, base + offset);
//     *dest = value;
// }

// uint32_t get_at(Mem_T *mem, uint32_t base, uint32_t offset)
// {
//     // convert v^2 address to virtual address
//     uint32_t *src = (uint32_t*) convert_address(mem, base + offset);
//     return *src;
// }


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
//     set_at(mem, base, offset, value);
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
//     return get_at(mem, base, offset);
// }

void vs_free(Mem_T *mem, uint32_t addr)
{
    // printf("Freeing the memory segment with id %u\n", addr);

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
