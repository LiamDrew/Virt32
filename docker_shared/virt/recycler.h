#ifndef RECYCLER_H
#define RECYCLER_H

#include <stdlib.h>
#include "mem_state.h"
#include "stack.h"



void* recycler_init(void);
// based on the size of the segment we want, calculate the index of the queue 
// using the log and bitshifts
uint32_t find_freed_segment(Mem_T *mem, uint32_t size);

void free_segment(Mem_T *mem, uint32_t seg_addr);



#endif
