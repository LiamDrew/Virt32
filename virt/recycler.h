#ifndef RECYCLER_H
#define RECYCLER_H

#include <stdlib.h>

typedef struct {
    uint32_t size;
    void *addr;
} Memory_t;

void *find_freed_segment(void);

#endif
