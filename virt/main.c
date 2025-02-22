#include <stdio.h>
#include "driver.h"
#include "mem_state.h"

#include "stack.h"
#include "recycler.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint32_t kernel_size = 100000;

    Mem_T *mem = init_memory_system(kernel_size);

    // vs_malloc
    uint32_t mal_size = 100;
    uint32_t first_malloc_result = vs_malloc(mem, mal_size);

    uint32_t res2 = vs_malloc(mem, 25);

    printf("Address of first malloc %u\n", first_malloc_result);
    printf("Address of second malloc %u\n", res2);

    uint32_t a1 = get_blocks_from_alloc_size(23);
    uint32_t a2 = get_blocks_from_alloc_size(24);
    uint32_t a3 = get_blocks_from_alloc_size(25);

    printf("Block size should be 1 %u\n", a1);
    printf("Block size should be 1 %u\n", a2);
    printf("Block size should be 2 %u\n", a3);
    vs_free(mem, first_malloc_result);

    /* Stack Tests */
    Stack *s = stack_init(10);
    stack_push(s, 1);
    stack_push(s, 2);
    stack_push(s, 3);
    stack_push(s, 4);
    stack_push(s, 5);
    stack_push(s, 6);
    stack_push(s, 7);
    stack_push(s, 8);
    stack_push(s, 9);
    stack_push(s, 10);
    stack_push(s, 11);
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));

    stack_free(s);

    /* Recycler Tests */
    // Mem_T mem;
    // mem.mem = NULL;
    // mem.recycler = recycler_init();

    // assert(stack_is_empty(&((Stack*)mem.recycler)->stack));
    // free_segment(&mem, 32);

    // (void)recycler;

    printf("Hi there\n");
    return 0;
}
