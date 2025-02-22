#include <stdlib.h>

typedef struct {
    uint32_t capacity;
    uint32_t size;
} Memory_t;

typedef struct {
    Memory_t* stack;
    uint32_t size;
    uint32_t capacity;
} Stack;