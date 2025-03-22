/**
 * @file driver.h
 * @date February 2025
 * @brief
 * The interface for initializing and using the virtual memory system.
 */

#ifndef DRIVER_H
#define DRIVER_H

#include "recycler.h"
#include <assert.h>
#include <string.h>

extern uint8_t *usable;
extern Stack_T *rec;
extern Mem_T *mem;
extern uint32_t start_unused;

uint8_t *init_memory_system(uint32_t kernel_size);

void terminate_memory_system(void);

/* Kernel (Re)allocate (kern_realloc):
 * Overwrite the zero segment and initialize all memory to zero */
uint32_t kern_realloc(uint32_t size);

/* Kernel Memory Copy (kern_memcpy):
 * Copies data from "userspace" to "kernel space" */
void kern_memcpy(uint32_t src_addr, uint32_t copy_size);

/* Virtual Segment Calloc (vs_calloc): 
 * Carve out a segment of virtual memory and serve it to the program as
 * zeroed-out v^2 memory */
uint32_t vs_calloc(uint32_t size);

/* Virtual Segment Free (vs_free):
 * Free a virtual segment for future use. */
void vs_free(uint32_t addr);

/* Set At (set_at):
 * Store a uint32_t at a virtual address */
inline void set_at(uint8_t *umem, uint32_t addr, uint32_t value)
{
    uint32_t *dest = convert_address(umem, addr);
    *dest = value;
}

/* Get At (get_at):
 * Get the uint32_t stored at a virtual address */
inline uint32_t get_at(uint8_t *umem, uint32_t addr)
{
    uint32_t *src = convert_address(umem, addr);
    return *src;
}

// void safe_set_at(Mem_T *mem, uint32_t base, uint32_t offset, uint32_t value);

// uint32_t safe_get_at(Mem_T *mem, uint32_t base, uint32_t offset);

#endif
