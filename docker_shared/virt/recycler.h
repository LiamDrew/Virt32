#ifndef RECYCLER_H
#define RECYCLER_H

#define SEG_NOT_FOUND 1

#include "mem_state.h"
#include "stack.h"
#include <stdlib.h>

Stack_T *recycler_init(void);

uint32_t find_freed_segment(uint32_t size);

void free_segment(uint8_t *umem, uint32_t seg_addr);

#endif
