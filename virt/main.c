#include <stdio.h>
#include "driver.h"
#include "stack.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    init_memory_system(0);

    // vs_malloc
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
    stack_free(s);
    


    printf("Hi there\n");
    return 0;
}
