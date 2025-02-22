/**
 * @file driver.h
 * @author Goat hackathon team
 * @date February 2025
 * @brief
 * The interface for initializing and using the virtual memory system. This
 * module is the software equivalent of the device driver, if we were doing
 * this project closer to the metal.
 */

#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include "mem_state.h"

// Initialize the virtual memory system
Mem_T *init_memory_system(uint32_t kernel_size);

/* Kernel malloc (kern_malloc): Overwrite whatever is in the zero
 * segment, regardless of whether something has been allocated there or not.
 * (This will be used for the Load Program instruction in the UM).
 */
void *kern_malloc(uint32_t size);

/* Kernel realloc (kern_realloc): Try to reallocate the zero segment (kernel
 * memory). Probably overwrite everything there. Fail if the user didn't leave 
 * enough space for the kernel on init */
void *kern_realloc(uint32_t size, void *addr);

/* Virtual segment malloc (vs_malloc): Carve out a segment of physical memory 
 * and serve it to the program as virtual memory. */
uint32_t vs_malloc(Mem_T *mem, uint32_t size);

// /* Virtual segment calloc (vs_calloc): Same thing as vs_malloc, but zero all
//  * the memory needed ahead of time */
// void *vs_calloc(uint32_t size);

void *convert_address(void);

/* Virtual segment free (vs_free): Free a virtual segment for future use. */
void vs_free(Mem_T *mem, uint32_t addr);



#endif
