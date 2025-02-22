#ifndef MEM_STATE_H
#define MEM_STATE_H

typedef struct {
    void *mem;              // pointer to the beggining of a 4GB memory segment
    void *recycler;         // data structure holding the free segment tags/sizes
    uint32_t kernel_size;   // kernel_size 
    uint32_t beg_unused;
} Mem_T;

#endif
