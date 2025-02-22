#include <stdio.h>
#include "driver.h"
#include "stack.h"
#include "recycler.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    init_memory_system(0);

    // vs_malloc

    /* Stack Tests */
    Stack* s = stack_init(10);
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
