#include "recycler.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define INIT_STACK_SIZE 2

Stack_T *recycler_init(void)
{
    Stack_T *recycler = malloc(sizeof(Stack_T) * REC_BUCKETS);
    assert(recycler != NULL);

    for (uint32_t i = 0; i < REC_BUCKETS; i++)
    {
        Stack_T s = stack_init(INIT_STACK_SIZE);
        recycler[i] = s;
    }

    return recycler;
}
