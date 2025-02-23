#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "recycler.h"
#include "driver.h"
#include "stack.h"
#include "mem_state.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* Recycler Tests */
    uint32_t kernel_size = 100000;
    Mem_T *mem = init_memory_system(kernel_size);
    (void)mem;

    mem->recycler = recycler_init();

    Stack *s = &((Stack*)mem->recycler)[0];
    assert(stack_is_empty(s));
    free_segment(mem, 32);
    assert(!stack_is_empty(s));
    assert(stack_top(s) == 24);

    // Finding freed segment at the initial index
    uint32_t segment = find_freed_segment(mem, 32);
    printf("segment: %d\n", segment);
    assert(segment == 24);

    // Finding freed segment at the next index
    assert(stack_is_empty(s));
    free_segment(mem, 64);
    s = &((Stack*)mem->recycler)[1];

    assert(!stack_is_empty(s));
    assert(stack_top(s) == 56);

    segment = find_freed_segment(mem, 32);
    printf("segment: %d\n", segment);
    assert(segment == 56);

    // Finding freed segment on empty freelists
    segment = find_freed_segment(mem, 32);
    assert((int)segment == -1);

    stack_free(s);

    // (void)recycler;
}
