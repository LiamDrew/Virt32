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

void *convert_address(Mem_T *mem, uint32_t addr);

// Initialize the virtual memory system
Mem_T *init_memory_system(uint32_t kernel_size);

/* Kernel recalloc (kern_recalloc):
 * Overwrite whatever is in the zero segment and initialize all requested
 * memory to zero. In a real OS, the kernel memory management needs to be
 * significantly more complicated, but since we are designing this memory system
 * for use in the UM.
 */

uint32_t kern_recalloc(Mem_T *mem, uint32_t size);

void kern_memcpy(Mem_T *mem, uint32_t src_addr, uint32_t dest_addr, uint32_t copy_size);

/* Virtual segment malloc (vs_malloc): Carve out a segment of physical memory 
 * and serve it to the program as virtual memory. */
uint32_t vs_malloc(Mem_T *mem, uint32_t size);

// /* Virtual segment calloc (vs_calloc): Same thing as vs_malloc, but zero all
//  * the memory needed ahead of time */
uint32_t vs_calloc(Mem_T *mem, uint32_t size);

void set_at(Mem_T *mem, uint32_t base, uint32_t offset, uint32_t value);

uint32_t get_at(Mem_T *mem, uint32_t base, uint32_t offset);

void safe_set_at(Mem_T *mem, uint32_t base, uint32_t offset, uint32_t value);

uint32_t safe_get_at(Mem_T *mem, uint32_t base, uint32_t offset);

/* Virtual segment free (vs_free): Free a virtual segment for future use. */
void vs_free(Mem_T *mem, uint32_t addr);



#endif
