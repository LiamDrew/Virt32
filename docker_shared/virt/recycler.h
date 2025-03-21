#ifndef RECYCLER_H
#define RECYCLER_H

#include "mem_state.h"
#include "stack.h"
#include <stdlib.h>

Stack_T *recycler_init(void);

uint32_t find_freed_segment(Mem_T *mem, uint32_t size);

void free_segment(Mem_T *mem, uint32_t seg_addr);

#endif
